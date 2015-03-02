using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using System.Windows.Forms;

namespace GLEditor.Engine
{
    class EnginePipeline
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern SafeFileHandle CreateNamedPipe(
           String pipeName,
           uint dwOpenMode,
           uint dwPipeMode,
           uint nMaxInstances,
           uint nOutBufferSize,
           uint nInBufferSize,
           uint nDefaultTimeOut,
           IntPtr lpSecurityAttributes);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern int ConnectNamedPipe(
           SafeFileHandle hNamedPipe,
           IntPtr lpOverlapped);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto,
        CallingConvention = CallingConvention.StdCall,
        SetLastError = true)]
        public static extern SafeFileHandle CreateFile(
            string lpFileName,
            uint dwDesiredAccess,
            uint dwShareMode,
            IntPtr SecurityAttributes,
            uint dwCreationDisposition,
            uint dwFlagsAndAttributes,
            IntPtr hTemplateFile
        );

        public const uint GENERIC_READ = (uint)0x80000000L;
        public const uint FILE_SHARE_READ = 0x00000001;
        public const uint FILE_SHARE_WRITE = 0x00000002;
        public const string PIPE_NAME = "\\\\.\\pipe\\EngineToEditorPipeline";
        public const int BUFFER_SIZE = 4096;
        public const uint OPEN_EXISTING = 3;
        public const uint FILE_ATTRIBUTE_NORMAL = 0x00000080;

        private Thread m_inboundReadThread;
        private SafeFileHandle m_inboundPipeHandle;
        private FileStream m_inboundFileStream;
        private byte[] m_inboundReadBuffer;

        public EnginePipeline()
        {
            m_inboundReadThread = new Thread(new ThreadStart(InboundReadThread));

            Console.Write("Connecting to pipe \n");

            m_inboundPipeHandle = CreateFile(
                    PIPE_NAME,
                    GENERIC_READ, // only need read access
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    IntPtr.Zero,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    IntPtr.Zero
                );
            if (m_inboundPipeHandle.IsInvalid)
            {
                Console.Write("Failed to connect to pipe\n");
                return;
            }
            m_inboundReadBuffer = new byte[EngineCommands.MAX_COMMAND_SIZE];
            m_inboundFileStream = new FileStream(m_inboundPipeHandle, FileAccess.ReadWrite, BUFFER_SIZE, false);
            m_inboundReadThread.Start();

            Application.ApplicationExit += new EventHandler(this.OnApplicationExit);
        }

        private void InboundReadThread()
        {
            int read;
            while ((read = m_inboundFileStream.Read(m_inboundReadBuffer, 0, m_inboundReadBuffer.Length)) > 0)
            {
                EngineCommands.Command command = new EngineCommands.Command();
                command.commandID = BitConverter.ToUInt32(m_inboundReadBuffer, 0);
                command.numBytes = BitConverter.ToUInt32(m_inboundReadBuffer, 4);
                Debug.Assert(read == command.numBytes + 8);
                if (command.numBytes > 0)
                {
                    command.bytes = new byte[command.numBytes];
                    Buffer.BlockCopy(m_inboundReadBuffer, 8, command.bytes, 0, (int)command.numBytes);
                }
                OnReceiveCommand(command);
                
                if (m_inboundReadThread.ThreadState == System.Threading.ThreadState.AbortRequested)
                    break;
            }
        }

        private void OnReceiveCommand(EngineCommands.Command a_command)
        {
            switch ((EngineCommands.CommandType) a_command.commandID)
            {
                case EngineCommands.CommandType.TEST0:
                    Console.Write("received test0\n");
                    break;
                case EngineCommands.CommandType.TEST1:
                    Console.Write("Received test1\n");
                    break;
                case EngineCommands.CommandType.TEST2:
                    break;
                case EngineCommands.CommandType.TEST3:
                    break;
                default:
                    Console.Write("Received invalid commandID: {0}\n", a_command.commandID);
                    break;
            }
        }

        private void OnApplicationExit(object sender, EventArgs e)
        {
            m_inboundPipeHandle.Close();
            m_inboundFileStream.Dispose();
            m_inboundReadThread.Abort();
        }
    }
}
