**题目4：通过Linux下的的套接字（SOCKET）编程实现一个简单的ECHO服务器，其中包括服务器与客户端两个应用程序，客户端向服务器发送一串字符到服务器，然后服务器收到字符后把他发回给客户端，并在字符串前加入客户端的IP地址信息。 参考：socket，listen，accept，send，recv，connect等。**

具体实现：

1、分别在Server和Client分支中make后，先启动Server端，再执行Client端去连接

