using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GLEditor.Engine
{
    class EngineCommands
    {
        public const uint MAX_COMMAND_SIZE = 1024;

        public enum CommandType
        {
            TEST0 = 0,
            TEST1 = 1,
            TEST2 = 2,
            TEST3 = 3
        };

        public class Command
        {
            public uint commandID;
            public uint numBytes;
            public byte[] bytes;
        }
    }
}
