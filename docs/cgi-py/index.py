import sys

sys.stdout.write("Content-type: text/html\r\n")
sys.stdout.write("Status: 200 OK\r\n")
sys.stdout.write("\r\n")

sys.stdout.write("<html><head><title>Test CGI Script</title></head>")
sys.stdout.write("<body>")
sys.stdout.write("<h1>Hello, World!</h1>")
sys.stdout.write("</body></html>")