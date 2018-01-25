using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net.Sockets;
using System.Net;

namespace GLEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Socket senderSock;

        public MainWindow()
        {
            InitializeComponent();

            // Create one SocketPermission for socket access restrictions  
            SocketPermission permission = new SocketPermission(
                NetworkAccess.Connect,    // Connection permission  
                TransportType.Tcp,        // Defines transport types  
                "",                       // Gets the IP addresses  
                SocketPermission.AllPorts // All ports  
                );

            // Ensures the code to have permission to access a Socket  
            permission.Demand();

            // Gets first IP address associated with a localhost  
            IPAddress ipAddr = IPAddress.Loopback;

            // Creates a network endpoint  
            IPEndPoint ipEndPoint = new IPEndPoint(ipAddr, 23232);

            // Create one Socket object to setup Tcp connection  
            senderSock = new Socket(
                ipAddr.AddressFamily,// Specifies the addressing scheme  
                SocketType.Stream,   // The type of socket   
                ProtocolType.Tcp     // Specifies the protocols   
                );

            senderSock.NoDelay = false;   // Using the Nagle algorithm  

            // Establishes a connection to a remote host  
            senderSock.Connect(ipEndPoint);
        }

        private void WindowCloseButton_Click(object sender, RoutedEventArgs e)
        {
            senderSock.Close();
            Close();
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            byte[] buffer = Encoding.UTF8.GetBytes("eheuheuehue" + Char.MinValue);
            senderSock.Send(buffer);
        }
            
    }
}
