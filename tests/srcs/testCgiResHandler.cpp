#include <gtest/gtest.h>
#include "TestEnv.hpp"
#include "CgiResponse.hpp"
#include <string>
#include "CgiResHandler.hpp"

namespace
{
class CgiResHandlerTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}
    virtual void TearDown()
    {
    }
};

// ========================================================
// =================== DocumentResponse ===================
// ========================================================

// document-response = Content-Type [ Status ] *other-field NL response-body
TEST_F(CgiResHandlerTest, HandleDocumentResponse)
{
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string cgiResponseMessage =	"Content-Type: text/html\r\n"
                                    "\r\n"
                                    "12345678890";
    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);

    CgiResHandler cgiResHandler;
    HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
    Response *response = dynamic_cast<Response *>(message);
    if (response == NULL)
        FAIL();
    EXPECT_EQ(response->getBody(), "12345678890");
    EXPECT_EQ(response->getHeader("Content-Type"), "text/html");
    delete cgiResponse;
}

TEST_F(CgiResHandlerTest, HandleDocumentResponseWithStatus)
{
    CgiResponse *cgiResponse = new CgiResponse();
    std::string cgiResponseMessage = "Status: 201 HOGE\r\nContent-Type: text/html\r\n\r\n12345678890";
    cgiResponse->parsing(cgiResponseMessage, 0);
    cgiResponse->getType();

    CgiResHandler cgiResHandler;
    HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
    Response *response = dynamic_cast<Response *>(message);
    if (response == NULL)
        FAIL();
    EXPECT_EQ(response->getBody(), "12345678890");
    EXPECT_EQ(response->getHeader("Content-Type"), "text/html");
    EXPECT_EQ(response->getStatus(), "201");
	EXPECT_EQ(response->getStatusMessage(), "HOGE");

    delete cgiResponse;
}

TEST_F(CgiResHandlerTest, HandleDocumentResponseError1)
{
    // without Content-Type
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);
    std::string cgiResponseMessage = "Status: 200 OK\r\n";
    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::InvalidResponse);

    CgiResHandler cgiResHandler;
    HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
    Response *response = dynamic_cast<Response *>(message);
    if (response == NULL)
        FAIL();
    EXPECT_EQ(response->getStatus(), "500");
    delete cgiResponse;
}


// // ========================================================
// // =================== LocalRedirectResponse ==============
// // ========================================================

// // local-redir-response = local-Location NL
// // local-Location  = "Location:" local-pathquery NL

TEST_F(CgiResHandlerTest, HandleLocalRedirectResponse)
{
    std::string cgiResponseMessage = "Location: /index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::LocalRedirectResponse);

    CgiResHandler cgiResHandler;
	CgiSocket *cgiSocket = new CgiSocket(50, env->socket);
    HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse, cgiSocket);
	if (message == NULL)
	{
		std::cout << "fjdsaiiosjeaw" << std::endl;
		FAIL();
	}
    Request *request = dynamic_cast<Request *>(message);
    request->setInfo();
    if (request == NULL)
    {
		std::cout << "test1" << std::endl;
		FAIL();
	}
	EXPECT_EQ(request->getUri(), "/index.html");
	EXPECT_EQ(request->getMethod(), "GET");
	EXPECT_EQ(request->getProtocol(), "HTTP/1.1");




    delete cgiResponse;
}

// // ========================================================
// // =================== ClientRedirectResponse =============
// // ========================================================

// client-redir-response = client-Location *extension-field NL
// client-Location = "Location:" fragment-URI NL

TEST_F(CgiResHandlerTest, HandleClientRedirectResponse)
{
    std::string cgiResponseMessage =  "Location: http://www.example.org/index.html\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);
    EXPECT_EQ(cgiResponse->getType(), CgiResponse::ClientRedirectResponse);

    CgiResHandler cgiResHandler;
    HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
    Response *response = dynamic_cast<Response *>(message);
    if (response == NULL)
        FAIL();

    EXPECT_EQ(response->getStatus(), "302");
	EXPECT_EQ(response->getHeader("Location"), "http://www.example.org/index.html");

    delete cgiResponse;
}

TEST_F(CgiResHandlerTest, HandleClientRedirectResponseWithExt)
{
    std::string cgiResponseMessage =  "Location: http://www.example.org/index.html\r\n"
							"X-CGI-TEST: hoge\r\n"
                            "\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);

	CgiResHandler cgiResHandler;
	HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
	Response *response = dynamic_cast<Response *>(message);
	if (response == NULL)
		FAIL();

    EXPECT_EQ(response->getStatus(), "302");
	EXPECT_EQ(response->getHeader("Location"), "http://www.example.org/index.html");

    delete cgiResponse;
}

// ========================================================
// =================== ClientRedirectResponseWithDocument =
// ========================================================

// client-redir-doc-response = client-Location Status Content-Type *other-field NL response-body

TEST_F(CgiResHandlerTest, HandleClientRedirectResponseWithDocument)
{
    std::string cgiResponseMessage = "Status: 200 OK\r\n"
                            "Location: http://www.example.org/index.html\r\n"
                            "Content-Type: text/html\r\n"
							"X-CGI-TEST: hoge\r\n"
                            "\r\n"
                            "1234567890\r\n";
    CgiResponse *cgiResponse = new CgiResponse();
    EXPECT_TRUE(cgiResponse != NULL);

    EXPECT_EQ(cgiResponse->parsing(cgiResponseMessage, 0), 0);

	CgiResHandler cgiResHandler;
	HttpMessage *message = cgiResHandler.handleMessage(*cgiResponse);
	Response *response = dynamic_cast<Response *>(message);
	if (response == NULL)
		FAIL();

	EXPECT_EQ(cgiResponse->getHeader("Status"), "200 OK");
	EXPECT_EQ(cgiResponse->getHeader("Location"), "http://www.example.org/index.html");
	EXPECT_EQ(cgiResponse->getHeader("Content-Type"), "text/html");
	EXPECT_EQ(cgiResponse->getHeader("X-CGI-TEST"), "hoge");
	EXPECT_EQ(cgiResponse->getBody(), "1234567890\r\n");

	delete cgiResponse;
}
}
