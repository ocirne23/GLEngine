using GLEditor.Engine;
using GLEditor.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


class Editor
{
    private const String ENGINE_APP_CLASS_NAME = "GLApp";
    private const String ENGINE_WINDOW_NAME = "GLEngine";

    private IntPtr m_glEngineHWND = IntPtr.Zero;

    public Editor()
    {
        EnginePipeline p = new EnginePipeline();
    }

    public IntPtr GetEngineHWND()
    {
        if (m_glEngineHWND == IntPtr.Zero)
        {
            m_glEngineHWND = WindowUtils.GetHWNDForName(ENGINE_APP_CLASS_NAME, ENGINE_WINDOW_NAME);
        }
        return m_glEngineHWND;
    }
}

