using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;

using LR;

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

        private readonly string ComboBoxThreadTemplateString = "Thread ";
        private readonly string ComboBoxMainThreadTemplateString = "Main thread";


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

                string hw = "hello world";
                MfcDll.mapsend(2, hw);

                MainThreadConsole = Process.Start("cpp_console.exe");

                comboBox_threadList.Items.Add(ComboBoxMainThreadTemplateString);

                
            }
            else
            {
                for (int i = 0; i < numericUpDown_threadCounter.Value; ++i)
                {
                    StartEvent.Set();
                    ConfirmEvent.WaitOne();

                    ThreadCounter++;
                    comboBox_threadList.Items.Add(ComboBoxThreadTemplateString + ThreadCounter.ToString());
                    comboBox_threadList.Refresh();
                }
            }
        }


        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (MainThreadConsole == null || MainThreadConsole.HasExited)
                return;

            for (int i = 0; i < numericUpDown_threadCounter.Value; i++)
            {
                if (ThreadCounter > 0)
                {
                    StopEvent.Set();
                    ConfirmEvent.WaitOne();

                    comboBox_threadList.Items.Remove(ComboBoxThreadTemplateString + ThreadCounter.ToString());
                    ThreadCounter--;
                    comboBox_threadList.Refresh();
                }
                else
                {
                    CloseDialogueEvent.Set();
                    comboBox_threadList.Items.Remove(ComboBoxMainThreadTemplateString);
                    comboBox_threadList.Refresh();
                    break;
                }
            }
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (MainThreadConsole == null || MainThreadConsole.HasExited)
                return;
            else
            {
                CloseDialogueEvent.Set();
                comboBox_threadList.Items.Clear();
            }
        }
    }




}