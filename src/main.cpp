#include <cucumber-cpp/internal/CukeEngineImpl.hpp>
#include <cucumber-cpp/internal/CukeExport.hpp>
#include <cucumber-cpp/internal/connectors/wire/WireServer.hpp>
#include <cucumber-cpp/internal/connectors/wire/WireProtocol.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/filesystem.hpp>
#include <token_matcher.h>
#include <builder.h>
#include <ast_builder.h>
#include <compiler.h>
#include <file_reader.h>
#include <parser.h>
#include <source_event.h>
#include <gherkin_document_event.h>
#include <pickle_event.h>
#include <string_token_scanner.h>
#include <attachment_event.h>

namespace {

    void acceptWireProtocol(const std::string &host, int port, const std::string &unixPath, bool verbose) {
        using namespace ::cucumber::internal;
        CukeEngineImpl cukeEngine;
        JsonSpiritWireMessageCodec wireCodec;
        WireProtocolHandler protocolHandler(wireCodec, cukeEngine);
        boost::scoped_ptr<SocketServer> server;
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
        if (!unixPath.empty()) {
            UnixSocketServer *const unixServer = new UnixSocketServer(&protocolHandler);
            server.reset(unixServer);
            unixServer->listen(unixPath);
            if (verbose)
                std::clog << "Listening on socket " << unixServer->listenEndpoint() << std::endl;
        } else
#else
            // Prevent warning about unused parameter
            static_cast<void>(unixPath);
#endif
        {
            TCPSocketServer *const tcpServer = new TCPSocketServer(&protocolHandler);
            server.reset(tcpServer);
            tcpServer->listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
            if (verbose)
                std::clog << "Listening on " << tcpServer->listenEndpoint() << std::endl;
        }
        server->acceptOnce();
    }

    void processGherkinFeatureFiles(const boost::filesystem::path& gherkinFilePath) {
        setlocale(LC_ALL, "en_US.UTF-8");
        TokenMatcher* token_matcher = TokenMatcher_new(L"en");
        Builder* builder = AstBuilder_new();
        Parser* parser = Parser_new(builder);
        Compiler* compiler = Compiler_new();
        int return_code = 0;
        int result_code = 0;
        const char* gherkinFilePathString = gherkinFilePath.string().c_str();
        FileReader* file_reader = FileReader_new(gherkinFilePathString);
        SourceEvent* source_event = SourceEvent_new(gherkinFilePathString, FileReader_read(file_reader));
        FileReader_delete(file_reader);
        /*if (options.print_source_events) {
            Event_print((const Event*)source_event, stdout);
        }*/
        TokenScanner* token_scanner = StringTokenScanner_new(source_event->source);
        result_code = Parser_parse(parser, token_matcher, token_scanner);
        if (result_code == 0) {
            const GherkinDocumentEvent* gherkin_document_event = GherkinDocumentEvent_new(AstBuilder_get_result(builder, gherkinFilePathString));
            result_code = Compiler_compile(compiler, gherkin_document_event->gherkin_document, source_event->source);
            Event_delete((const Event*)gherkin_document_event);
            if (result_code == 0) {
                // print pickle events
                    while (Compiler_has_more_pickles(compiler)) {
                        const Event* pickle_event = (const Event*)PickleEvent_new(Compiler_next_pickle(compiler));
                        Event_print(pickle_event, stdout);
                        Event_delete(pickle_event);
                    }
            }
            else {
                return_code = result_code;
            }
        }
        else {
            return_code = result_code;
            while (Parser_has_more_errors(parser)) {
                Error* error = Parser_next_error(parser);
                AttachmentEvent* attachment_event = AttachmentEvent_new(gherkinFilePathString, error->location);
                AttacnmentEvent_transfer_error_text(attachment_event, error);
                Event_print((Event*)attachment_event, stdout);
                Event_delete((Event*)attachment_event);
                Error_delete(error);
            }
        }
        TokenScanner_delete(token_scanner);
        Event_delete((const Event*)source_event);

        Compiler_delete(compiler);
        Parser_delete(parser);
        AstBuilder_delete(builder);
        TokenMatcher_delete(token_matcher);
        (void) return_code;
    }

    void processGherkinFeatureDirectories(const std::vector<std::string> &featuresFolders) {
        for (std::vector<std::string>::const_iterator i = featuresFolders.cbegin(); i != featuresFolders.cend(); ++i) {
            const std::string &p = *i;
            using namespace boost::filesystem;
            try {
                if (exists(p)) {
                    for (directory_entry& x : recursive_directory_iterator(p)) {
                        path file = x.path();
                        if (is_regular_file(file) && (extension(file) == ".feature")) {
                            processGherkinFeatureFiles(file);
                        }
                    }

                } else
                    std::cout << p << " does not exist\n";
            }

            catch (const filesystem_error &ex) {
                std::cout << ex.what() << '\n';
            }
        }
    }


}

int CUCUMBER_CPP_EXPORT main(int argc, char **argv) {
    using boost::program_options::value;
    boost::program_options::options_description optionDescription("Allowed options");
    optionDescription.add_options()
            ("help,h", "help for cucumber-cpp")
            ("verbose,v", "verbose output")
#if defined(USE_GHERKIN_C)
            ("feature-folders", value<std::vector<std::string> >()->multitoken(), "folders with gherkin files")
#endif // defined(USE_GHERKIN_C)
            ("listen,l", value<std::string>(), "listening address of wireserver")
            ("port,p", value<int>(), "listening port of wireserver, use '0' (zero) to select an ephemeral port")
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
            ("unix,u", value<std::string>(), "listening unix socket of wireserver (disables listening on port)")
#endif
            ;
    boost::program_options::variables_map optionVariableMap;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionDescription),
                                  optionVariableMap);
    boost::program_options::notify(optionVariableMap);

    if (optionVariableMap.count("help")) {
        std::cerr << optionDescription << std::endl;
        exit(1);
    }

    std::string listenHost("127.0.0.1");
    if (optionVariableMap.count("listen")) {
        listenHost = optionVariableMap["listen"].as<std::string>();
    }

    int port = 3902;
    if (optionVariableMap.count("port")) {
        port = optionVariableMap["port"].as<int>();
    }

    std::string unixPath;
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
    if (optionVariableMap.count("unix")) {
        unixPath = optionVariableMap["unix"].as<std::string>();
    }
#endif

    bool verbose = false;
    if (optionVariableMap.count("verbose")) {
        verbose = true;
    }

#if defined(USE_GHERKIN_C)
    if (optionVariableMap.count("feature-folders")) {
        processGherkinFeatureDirectories(optionVariableMap["feature-folders"].as<std::vector<std::string> >());
    }
#endif // defined(USE_GHERKIN_C)

    try {
        acceptWireProtocol(listenHost, port, unixPath, verbose);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
