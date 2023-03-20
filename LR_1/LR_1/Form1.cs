using System.Diagnostics;

namespace LR_1
{
    public partial class Form1 : Form
    {
        private Process? MainThreadConsole = null;

        private EventWaitHandle StopEvent = new(false, EventResetMode.ManualReset, "Stop");
        private EventWaitHandle StartEvent = new(false, EventResetMode.ManualReset, "Start");
        private EventWaitHandle ConfirmEvent = new(false, EventResetMode.AutoReset, "Confirm");
        private EventWaitHandle CloseDialogueEvent = new(false, EventResetMode.ManualReset, "Close");

        private int ThreadCounter = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (MainThreadConsole == null || MainThreadConsole.HasExited)
            {
                comboBox_threadList.Items.Clear();
                ThreadCounter = 0;

                MainThreadConsole = Process.Start("cpp_console.exe");

                comboBox_threadList.Items.Add("Main thread");
            }
            else
            {

            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {

        }
    }
}