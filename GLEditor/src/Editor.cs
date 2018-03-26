using GLEditor.src.Net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

public class Editor
{
    bool isRunning = true;
    MainWindow mainWindow;
    EngineSocket socket;

    public Editor(MainWindow mainWindow)
    {
        this.mainWindow = mainWindow;
        mainWindow.button.Click += Button_Click;
        socket = new EngineSocket();
        socket.Connect("localhost", 23235);
        MainLoop();
    }

    private void Button_Click(object sender, System.Windows.RoutedEventArgs e)
    {
    }

    void Shutdown()
    {
        isRunning = false;
    }

    void MainLoop()
    {
        while (isRunning)
        {
            
            Thread.Sleep(1);
        }
    }
}

