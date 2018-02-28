using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
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


public partial class MainWindow : Window
{
    Thread mainThread;

    public MainWindow()
    {
        InitializeComponent();
        mainThread = new Thread(mainThreadFunc);
        mainThread.Start(this);
    }

    private static void mainThreadFunc(Object mainWindow)
    {
        new Editor((MainWindow) mainWindow);
    }

    private void WindowCloseButton_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }

    private void button_Click(object sender, RoutedEventArgs e)
    {
        string path = Directory.GetCurrentDirectory();
        const string editorPath = @"GLEditor\bin\Release";
        string workingDirectory = path.Substring(0, path.Length - editorPath.Length) + "GLApp";
        string exePath = workingDirectory + @"\bin\x64\Release\GLApp.exe";
        string cmdText = @"/C cd /d " + workingDirectory + " && " + exePath;
        System.Diagnostics.Process process = System.Diagnostics.Process.Start("cmd.exe", cmdText);
        Thread.Sleep(10000);
        process.CloseMainWindow();
    } 
}

