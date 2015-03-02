using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Forms;
using System.Diagnostics;

namespace GLEditor
{
    public partial class MainWindow : Window
    {
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, int uFlags);

        private Editor editor;
        private IntPtr hwnd;

        public MainWindow()
        {
            InitializeComponent();

            hwnd = new WindowInteropHelper(this).Handle;
            editor = new Editor();

            IntPtr glEngine = editor.GetEngineHWND();
            if (glEngine == IntPtr.Zero)
            {
                String errorStr = String.Format("Could not find GLEngine");
                var result = System.Windows.Forms.MessageBox.Show(errorStr, "Error", MessageBoxButtons.OK);
                if (result == System.Windows.Forms.DialogResult.OK)
                    System.Windows.Application.Current.Shutdown();
            }
        }

        private void ResizeButton_Click(object sender, RoutedEventArgs e)
        {
        }
    }
}
