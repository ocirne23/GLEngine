using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GLEditor.Utils
{
    class WindowUtils
    {
        private const int WM_GETTEXT = 0x000D;
        private const int WM_GETTEXTLENGTH = 0x000E;

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool EnumWindows(EnumWindowsProc callback, IntPtr extraData);
        [DllImport("user32.dll")]
        static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int ProcessId);
        [DllImport("user32.dll")]
        internal static extern int GetWindowText(IntPtr hWnd, [Out, MarshalAs(UnmanagedType.LPTStr)] StringBuilder lpString, int nMaxCount);
        [DllImport("user32.dll")]
        static extern int GetWindowTextLength(IntPtr hWnd);
        [DllImport("user32.dll")]
        public static extern int RegisterWindowMessage(string lpString);
        [DllImport("user32.dll", EntryPoint = "SendMessage")]
        public static extern bool SendMessage(IntPtr hWnd, uint Msg, int wParam, StringBuilder lParam);
        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr SendMessage(int hWnd, int Msg, int wparam, int lparam);

        private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

        private static IntPtr s_enumerateWindowsHWND = IntPtr.Zero;
        private static int s_pid = 0;

        private static String s_windowName;

        private static String GetControlText(IntPtr hWnd)
        {
            StringBuilder title = new StringBuilder();
            Int32 size = SendMessage((int)hWnd, WM_GETTEXTLENGTH, 0, 0).ToInt32();
            if (size > 0)
            {
                title = new StringBuilder(size + 1);
                SendMessage(hWnd, (int)WM_GETTEXT, title.Capacity, title);
            }
            return title.ToString();
        }

        private static bool EnumerateWindowsCallback(IntPtr hWnd, IntPtr lParam)
        {
            int pid = 0;
            GetWindowThreadProcessId(hWnd, out pid);

            if (pid == s_pid)
            {
                String windowName = GetControlText(hWnd);
                if (windowName == s_windowName)
                {
                    s_enumerateWindowsHWND = hWnd;
                    return false;
                }
            }
            return true;
        }

        private static IntPtr GetHWNDforPID(int pid)
        {
            s_pid = pid;
            s_enumerateWindowsHWND = IntPtr.Zero;
            EnumWindows(new EnumWindowsProc(EnumerateWindowsCallback), IntPtr.Zero);
            return s_enumerateWindowsHWND;
        }

        public static IntPtr GetHWNDForName(String a_className, String a_windowName)
        {
            s_windowName = a_windowName;

            Process[] processes = Process.GetProcessesByName(a_className);
            IntPtr hwnd = IntPtr.Zero;
            if (processes.Length > 0)
                hwnd = GetHWNDforPID(processes[0].Id);
            return hwnd;
        }
    }
}
