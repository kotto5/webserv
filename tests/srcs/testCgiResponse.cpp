#include <gtest/gtest.h>
#include "TestEnv.hpp"
#include "CgiResponse.hpp"
#include <string>

namespace
{
class CgiResponseTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}
    virtual void TearDown()
    {
    }
};

TEST_F(CgiResponseTest, CgiResponse)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    delete cgiResponse;
}

// document-response = Content-Type [ Status ] *other-field NL response-body

TEST_F(CgiResponseTest, DocumentResponse)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string Response = "Content-Type: text/html\r\n\r\n12345678890\r\n";
    EXPECT_EQ(cgiResponse->parsing(Response, 0), 0);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, DocumentResponseWithStatus)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string Response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n12345678890\r\n";
    EXPECT_EQ(cgiResponse->parsing(Response, 0), 0);
    delete cgiResponse;
}

// local-redir-response = local-Location NL
// local-Location  = "Location:" local-pathquery NL

TEST_F(CgiResponseTest, LocalRedirectResponse)
{
    std::string response = "Location: /index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    delete cgiResponse;
}

// client-redir-response = client-Location *extension-field NL
// client-Location = "Location:" fragment-URI NL

TEST_F(CgiResponseTest, ClientRedirectResponse)
{
    std::string response = "HTTP/1.1 200 OK\r\n"
                            "Location: http://www.example.org/index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    delete cgiResponse;
}

// //  client-redirdoc-response = client-Location Status Content-Type *other-field NL response-body

TEST_F(CgiResponseTest, ClientRedirectResponseWithDocument)
{
    std::string response = "HTTP/1.1 200 OK\r\n"
                            "Location: http://www.example.org/index.html\r\n"
                            "\r\n"
                            "1234567890\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    delete cgiResponse;
}


}
