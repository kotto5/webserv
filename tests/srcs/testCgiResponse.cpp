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

// ========================================================
// =================== DocumentResponse ===================
// ========================================================

// document-response = Content-Type [ Status ] *other-field NL response-body
TEST_F(CgiResponseTest, DocumentResponse)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string Response = "Content-Type: text/html\r\n\r\n12345678890\r\n";
    EXPECT_EQ(cgiResponse->parsing(Response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::DocumentResponse);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, DocumentResponseWithStatus)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string Response = "Status: 200 OK\r\nContent-Type: text/html\r\n\r\n12345678890";
    EXPECT_EQ(cgiResponse->parsing(Response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::DocumentResponse);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, DocumentResponseError1)
{
    // without Content-Type
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string Response = "Status: 200 OK\r\n";
    EXPECT_EQ(cgiResponse->parsing(Response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::InvalidResponse);
    delete cgiResponse;
}


// ========================================================
// =================== LocalRedirectResponse ==============
// ========================================================

// local-redir-response = local-Location NL
// local-Location  = "Location:" local-pathquery NL

TEST_F(CgiResponseTest, LocalRedirectResponse)
{
    std::string response = "Location: /index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::LocalRedirectResponse);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, LocalRedirectResponseError1)
{
    std::string response = "Location: /index.html\r\n"
                            "Status: 200 OK\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::InvalidResponse);
    delete cgiResponse;
}

// ========================================================
// =================== ClientRedirectResponse =============
// ========================================================

// client-redir-response = client-Location *extension-field NL
// client-Location = "Location:" fragment-URI NL

TEST_F(CgiResponseTest, ClientRedirectResponse)
{
    std::string response =  "Location: http://www.example.org/index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::ClientRedirectResponse);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, ClientRedirectResponseError1)
{
    std::string response =  "Location: http://www.example.org/index.html\r\n"
                            "Status: 200 OK\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::InvalidResponse);
    delete cgiResponse;
}

// ========================================================
// =================== ClientRedirectResponseWithDocument =
// ========================================================

// client-redir-doc-response = client-Location Status Content-Type *other-field NL response-body

TEST_F(CgiResponseTest, ClientRedirectResponseWithDocument)
{
    std::string response = "Status: 200 OK\r\n"
                            "Location: http://www.example.org/index.html\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "1234567890\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::ClientRedirectResponseWithDocument);
    delete cgiResponse;
}

TEST_F(CgiResponseTest, basicTest)
{
    std::string response = "Status: 200 OK\r\n"
                            "Location: http://www.example.org/index.html\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "1234567890\r\n";

    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(response, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::ClientRedirectResponseWithDocument);
    EXPECT_EQ(cgiResponse->getHeader("Status"), "200 OK");
    EXPECT_EQ(cgiResponse->getHeader("Location"), "http://www.example.org/index.html");
    EXPECT_EQ(cgiResponse->getHeader("Content-Type"), "text/html");
    EXPECT_EQ(cgiResponse->getBody(), "1234567890\r\n");
    delete cgiResponse;
}


}
