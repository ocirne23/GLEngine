using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Net;

namespace GLEditor.src.Net
{
    public class EngineSocket
    {
        Socket senderSock;

        public EngineSocket()
        {
            SocketPermission permission = new SocketPermission(NetworkAccess.Connect, TransportType.Tcp, "", SocketPermission.AllPorts);
            permission.Demand();
        }

        public void Connect(string ipAddrStr, int port)
        {
            IPAddress addr;
            try
            {
                if (ipAddrStr.ToLower().Equals("localhost"))
                {
                    addr = IPAddress.Loopback;
                }
                else
                {
                    addr = IPAddress.Parse(ipAddrStr);
                }
            }
            catch (FormatException)
            {
                return;
            }
            try
            {
                IPEndPoint ipEndPoint = new IPEndPoint(addr, port);
                senderSock = new Socket(addr.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                senderSock.NoDelay = false;
                senderSock.Connect(ipEndPoint);
            }
            catch (Exception)
            {
                if (senderSock.Connected)
                {
                    senderSock.Disconnect(true);
                    senderSock = null;
                }
                return;
            }
        }

        public void Disconnect()
        {
            if (senderSock.Connected)
            {
                senderSock.Disconnect(true);
                senderSock = null;
            }
        }

        public void Send(byte[] buffer, int size)
        {
            if (senderSock.Connected)
            {
                senderSock.Send(buffer, size, SocketFlags.None);
            }
        }

        public delegate void Del(out byte[] data);

        public void Receive(ref byte[] buffer, Del del)
        {
            if (senderSock.Connected)
            {
                senderSock.Receive(buffer, SocketFlags.None);
                del.Invoke(out buffer);
            }
        }
    }
}
