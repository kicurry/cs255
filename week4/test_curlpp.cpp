#include <iostream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

using namespace std;

// static string TARGET = "http://crypto-class.appspot.com/po?er=";
static string TARGET = "http://www.baidu.com";  // OK

int main()
{
    // string cipher_text = "f20bdba6ff29eed7b046d1df9fb7000058b1ffb4210a580f748b4ac714c001bd4a61044426fb515dad3f21f18aa577c0bdf302936266926ff37dbf7035d5eeb4";
    // string cipher_text = "f20bdba6ff29eed7b046d1df9fb7000058b1ffb4210a580f748";
    string cipher_text = "";
    string url = TARGET + cipher_text;
    try
    {
        curlpp::Cleanup clear;
        curlpp::Easy request;

        // setting the URL to retrive
        request.setOpt(new curlpp::options::Url(url));

        cout << request << endl;

        return EXIT_SUCCESS;
    }
    catch (curlpp::LogicError &e)
    {
        cerr << e.what() << endl;
    }
    catch (curlpp::RuntimeError &e)
    {
        cout << e.what() << endl;
    }

    return EXIT_FAILURE;
}