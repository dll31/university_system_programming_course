namespace LR_1
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            buttonStart = new Button();
            label1 = new Label();
            numericUpDown_threadCounter = new NumericUpDown();
            label2 = new Label();
            buttonStop = new Button();
            comboBox_threadList = new ComboBox();
            ((System.ComponentModel.ISupportInitialize)numericUpDown_threadCounter).BeginInit();
            SuspendLayout();
            // 
            // buttonStart
            // 
            buttonStart.Location = new Point(71, 86);
            buttonStart.Name = "buttonStart";
            buttonStart.Size = new Size(75, 23);
            buttonStart.TabIndex = 0;
            buttonStart.Text = "Start";
            buttonStart.UseVisualStyleBackColor = true;
            buttonStart.Click += buttonStart_Click;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(70, 22);
            label1.Name = "label1";
            label1.Size = new Size(73, 15);
            label1.TabIndex = 1;
            label1.Text = "Start threads";
            // 
            // numericUpDown_threadCounter
            // 
            numericUpDown_threadCounter.Location = new Point(71, 47);
            numericUpDown_threadCounter.Name = "numericUpDown_threadCounter";
            numericUpDown_threadCounter.Size = new Size(120, 23);
            numericUpDown_threadCounter.TabIndex = 2;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(328, 22);
            label2.Name = "label2";
            label2.Size = new Size(66, 15);
            label2.TabIndex = 3;
            label2.Text = "Threads list";
            // 
            // buttonStop
            // 
            buttonStop.Location = new Point(328, 86);
            buttonStop.Name = "buttonStop";
            buttonStop.Size = new Size(75, 23);
            buttonStop.TabIndex = 4;
            buttonStop.Text = "Stop";
            buttonStop.UseVisualStyleBackColor = true;
            buttonStop.Click += buttonStop_Click;
            // 
            // comboBox_threadList
            // 
            comboBox_threadList.FormattingEnabled = true;
            comboBox_threadList.Location = new Point(328, 47);
            comboBox_threadList.Name = "comboBox_threadList";
            comboBox_threadList.Size = new Size(121, 23);
            comboBox_threadList.TabIndex = 5;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(523, 149);
            Controls.Add(comboBox_threadList);
            Controls.Add(buttonStop);
            Controls.Add(label2);
            Controls.Add(numericUpDown_threadCounter);
            Controls.Add(label1);
            Controls.Add(buttonStart);
            Name = "Form1";
            Text = "Form1";
            FormClosing += Form1_FormClosing;
            Load += Form1_Load;
            ((System.ComponentModel.ISupportInitialize)numericUpDown_threadCounter).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button buttonStart;
        private Label label1;
        private NumericUpDown numericUpDown_threadCounter;
        private Label label2;
        private Button buttonStop;
        private ComboBox comboBox_threadList;
    }
}