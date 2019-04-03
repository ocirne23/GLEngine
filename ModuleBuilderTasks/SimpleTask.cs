using System;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace MyTasks
{
    public class SimpleTask : Task
    {
        public override bool Execute()
        {
            Console.Write("SIMPLE TASKS AYAYYAYA");
            return true;
        }

        public string MyProperty { get; set; }

    }
}