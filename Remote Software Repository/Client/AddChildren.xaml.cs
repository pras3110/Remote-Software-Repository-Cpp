

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddChildren.xaml.cs -Demonstrates Add Children to a Checkin file to Repository Functionality        //
// Author: Prasanna Dodwad, pdodwad@syr.edu                                                            //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                                       //
// Environment: C# console                                                                             //             
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * 
 *   Package Operations:
 *   -------------------
 *   This package defines the Add Children functionality for the project4. Its functionality is to:
 *   
 *   - Allow Client to add dependencies for a particular CheckIn
 *   
 *   
 *   This Package also defines AutoClosingMessageBox class :
 *    - To show error message on a message box
 *   
 *    * 
* Required Files:
* ---------------
* AddChildren.xaml , AddChildren.xaml.cs
* 
* 
* 
* Maintenance History:
* --------------------
* ver 1.0 : 1st May 2018
*





 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Client
{
    /// <summary>
    /// Interaction logic for AddChildren.xaml
    /// </summary>
    public partial class AddChildren : Window
    {

        private string file_name = "";
        private string namespace_spc = "";
        private string checkin_ver = "";
        private DBCore thisdb;

        List<String> childrenReturn(String childs)
        {
            List<String> result=new List<string>();
            int index = childs.IndexOf(";", 0);
            int start = 0;
            while (index != -1)
            {
                String temp = childs.Substring(start, index - start);
                result.Add(temp);
                start = index + 1;
                index = childs.IndexOf(";", start);
            }

            return result;

        }



        public AddChildren()
        {
            InitializeComponent();
        }


        // Initializes the Constructor for Database keys
        public AddChildren(DBCore keys,String filename,String name_spc,String checkinversion)
        {
            file_name = filename;
            namespace_spc = name_spc;
            checkin_ver = checkinversion;
            thisdb = keys;
            InitializeComponent();

            Dictionary<int,DBElement> db = keys.GetEntireDB;

            foreach (KeyValuePair<int, DBElement> entry in db)
            {
                String children="";

                if (entry.Value.ChilderenGetterSetter.Count() == 0)
                {
                    children = "";
                }
                else
                {
                    List<String> temp = entry.Value.ChilderenGetterSetter;
                    foreach (var x in temp)
                    {
                        children = children + x + ";";
                    }
                }


                Children_list.Items.Add(new Children
                {
                    Keys = entry.Value.KeyGetterSetter,
                    Children_Value=children

                });

            }



        }

        // delay functions 
        async Task putDelay1()
        {
            await Task.Delay(1000);
        }
        // delay functions 
        async Task putDelay2()
        {
            await Task.Delay(2000);
        }

        // Automation Test Case for Testing
        async public void Automate2()
        {
            Children_list.SelectedIndex = 3;
            var obj = Children_list.SelectedItem as Children;obj.CheckBoxSelected = true;
            await putDelay2();int flag = 0;
            string key = namespace_spc + "::" + file_name + "." + checkin_ver;
            foreach (var o in Children_list.Items)
            {
                var temp = o as Children;
                if (temp.CheckBoxSelected == true)
                {
                    if (temp.Children_Value != "")
                    {
                        List<string> children = childrenReturn(temp.Children_Value);
                        if (children.Contains(key))
                        {
                            flag = 1;
                            var x = temp;
                            x.CheckBoxSelected = false;
                        }
                    }
                }
            }
            if (flag == 1)
            {
                AutoClosingMessageBox.Show("On Or More Children selected will cause Circular Dependency !!", "AddChildren", 2000);
                Children_list.Items.Clear();Dictionary<int, DBElement> db = thisdb.GetEntireDB;
                foreach (KeyValuePair<int, DBElement> entry in db)
                {
                    String children = "";
                    if (entry.Value.ChilderenGetterSetter.Count() == 0)
                        children = "";
                    else
                    {
                        List<String> temp = entry.Value.ChilderenGetterSetter;
                        foreach (var x in temp)
                            children = children + x + ";";
                    }
                    Children_list.Items.Add(new Children
                    {
                        Keys = entry.Value.KeyGetterSetter,
                        Children_Value = children
                    });

                }
                this.Close();
            }
            else
                this.Close();
        }

        // Automation Test Case for Testing
        async public void Automate()
        {
            Children_list.SelectedIndex = 3;
            var obj = Children_list.SelectedItem as Children;obj.CheckBoxSelected = true;
            await putDelay2();int flag = 0;
            string key = namespace_spc + "::" + file_name + "." + checkin_ver;
            foreach (var o in Children_list.Items)
            {
                var temp = o as Children;
                if (temp.CheckBoxSelected == true)
                {
                    if (temp.Children_Value != "")
                    {
                        List<string> children = childrenReturn(temp.Children_Value);
                        if (children.Contains(key))
                        {
                            flag = 1;
                            var x = temp;
                            x.CheckBoxSelected = false;
                        }
                    }
                }
            }
            if (flag == 1)
            {
                AutoClosingMessageBox.Show("On Or More Children selected will cause Circular Dependency !!", "AddChildren", 2000);
                Children_list.Items.Clear();Dictionary<int, DBElement> db = thisdb.GetEntireDB;
                foreach (KeyValuePair<int, DBElement> entry in db)
                {
                    String children = "";
                    if (entry.Value.ChilderenGetterSetter.Count() == 0)
                        children = "";
                    else
                    {
                        List<String> temp = entry.Value.ChilderenGetterSetter;
                        foreach (var x in temp)
                            children = children + x + ";";
                    }
                    Children_list.Items.Add(new Children
                    {
                        Keys = entry.Value.KeyGetterSetter,
                        Children_Value = children
                    });
                }
                this.Close();
            }
            else
                this.Close();
        }


        // Event Handler for Add Children Button on MainWindow.xaml Checkin Button
        private void add_child_click(object sender, RoutedEventArgs e)
        {
            int flag = 0;
            string key = namespace_spc + "::" + file_name+"."+checkin_ver;
            foreach (var o in Children_list.Items)
            {
                var temp = o as Children;
                if (temp.CheckBoxSelected == true)
                {
                    if(temp.Children_Value != "")
                    {
                        List<string> children = childrenReturn(temp.Children_Value);
                        if (children.Contains(key))
                        {
                            flag = 1;
                            var x = temp;
                            x.CheckBoxSelected = false;
                        }
                    }
                }
            }
            if (flag == 1)
            {
                MessageBoxResult result = MessageBox.Show("On Or More Children selected will cause Circular Dependency !!", "AddChildren", MessageBoxButton.OK, MessageBoxImage.Error);
                Children_list.Items.Clear();
                Dictionary<int, DBElement> db = thisdb.GetEntireDB;
                foreach (KeyValuePair<int, DBElement> entry in db)
                {
                    String children = "";
                    if (entry.Value.ChilderenGetterSetter.Count() == 0)
                        children = "";
                    else
                    {
                        List<String> temp = entry.Value.ChilderenGetterSetter;
                        foreach (var x in temp)
                            children = children + x + ";";
                        
                    }
                    Children_list.Items.Add(new Children
                    {
                        Keys = entry.Value.KeyGetterSetter,
                        Children_Value = children
                    });
                }
            }
            else
                this.Close();
        }

        private void window_close(object sender, System.ComponentModel.CancelEventArgs e)
        {

        }
    }


    public class Children
    {
        public string Keys { get; set; }
        public bool CheckBoxSelected { get; set; }
        public string Children_Value { get; set; }

    }


    public class AutoClosingMessageBox
    {
        System.Threading.Timer _timeoutTimer;
        string _caption;
        AutoClosingMessageBox(string text, string caption, int timeout)
        {
            _caption = caption;
            _timeoutTimer = new System.Threading.Timer(OnTimerElapsed,
                null, timeout, System.Threading.Timeout.Infinite);
            using (_timeoutTimer)
                MessageBox.Show(text, caption);
        }
        public static void Show(string text, string caption, int timeout)
        {
            new AutoClosingMessageBox(text, caption, timeout);
        }
        void OnTimerElapsed(object state)
        {
            IntPtr mbWnd = FindWindow("#32770", _caption); // lpClassName is #32770 for MessageBox
            if (mbWnd != IntPtr.Zero)
                SendMessage(mbWnd, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
            _timeoutTimer.Dispose();
        }
        const int WM_CLOSE = 0x0010;
        [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
        [System.Runtime.InteropServices.DllImport("user32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto)]
        static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, IntPtr wParam, IntPtr lParam);
    }

}
