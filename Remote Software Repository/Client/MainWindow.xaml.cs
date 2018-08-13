

///////////////////////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs -Demonstrates Client User Functionality                        //
// Author: Prasanna Dodwad, pdodwad@syr.edu                                          //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                     //
// Environment: C# console                                                           //             
///////////////////////////////////////////////////////////////////////////////////////


/*
 * 
 *   Package Operations:
 *   -------------------
 *   This package defines the GUI functionality for the project3. Its functionality is to:
 *   
 *   - Provides various tabs like Connect,Browse,CheckIn,CheckOut,ViewFile,ViewMetadata for client
 *     functionality
 *   - Connect Tab : used to connect to the Server Repository
 *   - Browse Tab : used to browse the Server Repository and to display its content on new window
 *   - Checkin Tab : used to checkin a file from Client to Server Repository
 *   - Checkout Tab : used to checkout a file from Repository to Client

 *   
 *   This package has 4 classes
 *   
 *   1) partial class MainWindow
 *   --------------------------------
 *   The public methods in this Class are:
 *   
 *      - Test_1a() : Test Case for automated running
 *      - Test_1b() : Test Case for automated running
 *      - Test_1b1() : Test Case for automated running
 *      - Test_1c() : Test Case for automated running
 *      - Test_1d() : Test Case for automated running
 *      - secondGUI(string portno) : Test Function to run 2nd Client 
 *      
 *    2)   AutoClosingMessageBox class :
 *    - To show error message on a message box
 *    
 *    3) Class DBElement :
 *    - To Construct the DBElement on the Repository for Add Children Functionality
 *    
 *    4) Class DBCore :
 *    - Stores a Dictionary of DBElements
 * 
* Required Files:
* ---------------
* MainWindow.xaml , MainWindow.xaml.cs
* Translater.dll
* PopUp.xaml , PopUp.xaml.cs
* AddFolder.xaml , AddFolder.xaml.cs
* AddChildren.xaml , AddChildren.xaml.cs


* Maintenance History:
* --------------------
* ver 1.0 : 1st May 2018
 *   
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * */

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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;
using Microsoft.Win32;
using System.ComponentModel;

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

        // Represents the DBElement on the Repository to construct for Add Children for a Checkin
    public class DBElement
    {
        private String key;
        private List<String> children = new List<string>();

        public String KeyGetterSetter
        {
            get { return key; }
            set { key = value; }
        }

        public List<string> ChilderenGetterSetter
        {
            get { return children; }
            set { children = value; }
        }

    }

    // Defines a dictionary to store the DbElements
   public class DBCore
    {

        private Dictionary<int, DBElement> dbelements = new Dictionary<int, DBElement>();

        public void Set(int key, DBElement value)
        {
            if (dbelements.ContainsKey(key))
            {
                dbelements[key] = value;
            }
            else
            {
                dbelements.Add(key, value);
            }
        }

        public DBElement Get(int key)
        {
            DBElement result = null;

            if (dbelements.ContainsKey(key))
            {
                result = dbelements[key];
            }

            return result;
        }

        public Dictionary<int,DBElement> GetEntireDB
        {
            get{ return dbelements; }
            
        }


    }
   

    public partial class MainWindow : Window
    {
        private Button AddChildrenTest, AddChildrenTest2;
        public MainWindow()
        {
            InitializeComponent();
           

            
        }

        private string fullPathOfCheckin;
        private string SendPathForClient = "../../../../SendFilesClient";
        private string SavePathForClient = "../../../../LocalClientRepository";
        private Stack<string> pathStack_browse = new Stack<string>();
        private Stack<string> pathStack_metadata = new Stack<string>();
        private Stack<string> pathStack_checkout = new Stack<string>();
        private Stack<string> pathStack_checkin = new Stack<string>();
        private Stack<string> pathStack_file = new Stack<string>();
        //private List<string> children = new List<string>();
        
        private Translater translater;
        private CsEndPoint endPoint_;
        private CsEndPoint serverEndPoint;
        AddFolder inputDialog;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();

        // Child Thread to receive message from receiver queue
        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    if (msg.value("command") == "quitthread")
                    {
                        Console.WriteLine("Quitting client process messages");
                        translater.stop();
                        break;
                        
                    }
                    string msgId = msg.value("command");
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }

         // adds Action delegates to the dictionary
        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        private void listChildren(DBCore keys,string version)
        {
            string children = "";
            var filename1 = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename1.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }
            AddChildren c = new AddChildren(keys,file_name,str_nmspc,version);
            c.ShowDialog();
            foreach (var o in c.Children_list.Items)
            {
                var temp = o as Children;
                if (temp.CheckBoxSelected == true)
                {
                    children = children + temp.Keys + ";";
                }
            }
            var filename = FileList_checkin.SelectedItem as FileDetails;
            filename.Children_value = children;
            ListBoxItem myListBoxItem = (ListBoxItem)(FileList_checkin.ItemContainerGenerator.ContainerFromItem(FileList_checkin.SelectedItem));
            ContentPresenter myContentPresenter = FindVisualChild<ContentPresenter>(myListBoxItem);
            // Finding textBlock from the DataTemplate that is set on that ContentPresenter
            DataTemplate myDataTemplate = myContentPresenter.ContentTemplate;
            TextBox myTextBlock = (TextBox)myDataTemplate.FindName("children_tag", myContentPresenter);
            if (myTextBlock != null)
            {
                myTextBlock.Text = children;
                //children = null;
            }
            Console.WriteLine("Reached here");
        }

        // Adds children for a Automated Test Case
        private void listChildrenAuto(DBCore keys, string version)
        {
            var filename1 = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename1.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }
            AddChildren c = new AddChildren(keys, file_name, str_nmspc, version);
            c.Automate();
            c.ShowDialog();
            FileList_checkin.Items.Clear();
        }

        // Adds children for a Automated Test Case
        private void listChildrenAuto2(DBCore keys, string version)
        {

            string children = "";
            var filename1 = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename1.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }

            AddChildren c = new AddChildren(keys, file_name, str_nmspc, version);
            c.Automate2();
           c.ShowDialog();
            foreach (var o in c.Children_list.Items)
            {
                var temp = o as Children;
                if (temp.CheckBoxSelected == true)
                {
                    children = children + temp.Keys + ";";
                }
            }
           var filename = FileList_checkin.SelectedItem as FileDetails;
            filename.Children_value = children;
           ListBoxItem myListBoxItem = (ListBoxItem)(FileList_checkin.ItemContainerGenerator.ContainerFromItem(FileList_checkin.SelectedItem));
            ContentPresenter myContentPresenter = FindVisualChild<ContentPresenter>(myListBoxItem);
           // Finding textBlock from the DataTemplate that is set on that ContentPresenter
            DataTemplate myDataTemplate = myContentPresenter.ContentTemplate;
            TextBox myTextBlock = (TextBox)myDataTemplate.FindName("children_tag", myContentPresenter);
            if (myTextBlock != null)
            {
                myTextBlock.Text = children;
            }
           Console.WriteLine("after ends");
            string temp23 = PathTextBlock_checkin.Text;
            checkin_click(this,null);
        }




        // 
        private childitem FindVisualChild<childitem>(DependencyObject obj) where childitem : DependencyObject
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(obj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(obj, i);
                if (child != null && child is childitem)
                    return (childitem)child;
                else
                {
                    childitem childOfChild = FindVisualChild<childitem>(child);
                    if (childOfChild != null)
                        return childOfChild;
                }
            }
            return null;
        }


        // adds message to the TextBox in connect tab
        private void addMsgContent_Connect(string msg)
        {
            display_Text_connect.Text =  display_Text_connect.Text+"\n"+msg;

        }
        // adds message to the TextBox in browse tab
        private void addMsgContent_browse(string msg)
        {
            display_Text_browse.Text = display_Text_browse.Text +"\n"+ msg;
        }

        // adds message to the TextBox in metadata tab
        private void addMsgContent_metadata(string msg)
        {
            display_Text_metadata.Text = display_Text_metadata.Text + "\n" + msg;
        }

        // adds message to the TextBox in checkout tab
        private void addMsgContent_checkout(string msg)
        {
            display_Text_checkout.Text = display_Text_checkout.Text + "\n" + msg;
        }
        // adds message to the TextBox in checkin tab
        private void addMsgContent_checkin(string msg)
        {
            display_Text_checkin.Text = display_Text_checkin.Text + "\n" + msg;
        }

        // adds message to the TextBox in file tab
        private void addMsgContent_file(string msg)
        {
            display_Text_file.Text = display_Text_file.Text + "\n" + msg;
        }

        

        // clears the Dirs Listbox in metadata tab
        private void clearDirs_Metadata()
        {
            DirList_metadata.Items.Clear();
        }
        // clears the Dirs Listbox in checkout tab
        private void clearDirs_Checkout()
        {
            DirList_checkout.Items.Clear();
        }

        // clears the Dirs Listbox in checkin tab
        private void clearDirs_Checkin()
        {
            DirList_checkin.Items.Clear();
        }
        // clears the Dirs Listbox in file tab
        private void clearDirs_File()
        {
            DirList_file.Items.Clear();
        }

        private void clearFiles_Checkin()
        {
            FileList_checkin.Items.Clear();
        }


        

        // adds item in the Dirs Listbox in metadata tab
        private void addDir_Metadata(string dir)
        {
            DirList_metadata.Items.Add(dir);
        }
        // adds item in the Dirs Listbox in checkout tab
        private void addDir_Checkout(string dir)
        {
            DirList_checkout.Items.Add(dir);
        }
        // adds item in the Dirs Listbox in checkin tab
        private void addDir_Checkin(string dir)
        {
            DirList_checkin.Items.Add(dir);
        }

        // adds item in the Dirs Listbox in file tab
        private void addDir_File(string dir)
        {
            DirList_file.Items.Add(dir);
        }


       

        // inserts ../ directory in metadata tab
        private void insertParent_Metadata()
        {
            DirList_metadata.Items.Insert(0, "..");
        }

        // inserts ../ directory in checkout tab
        private void insertParent_Checkout()
        {
            DirList_checkout.Items.Insert(0, "..");
        }

        // inserts ../ directory in checkin tab
        private void insertParent_Checkin()
        {
            DirList_checkin.Items.Insert(0, "..");
        }
        // inserts ../ directory in file tab
        private void insertParent_File()
        {
            DirList_file.Items.Insert(0, "..");
        }


        // clears the Files Listbox in browse tab
        private void clearFiles_Browse()
        {
            FileList_browse.Items.Clear();
        }

    // clears the Files Listbox in metadata tab
    private void clearFiles_Metadata()
        {
            FileList_metadata.Items.Clear();
        }

        // clears the Files Listbox in checkout tab
        private void clearFiles_Checkout()
        {
            FileList_checkout.Items.Clear();
        }
        // clears the Files Listbox in file tab
        private void clearFiles_File()
        {
            FileList_file.Items.Clear();
        }



        // adds item to Files Listbox in browse tab
        private void addFile_Browse(string file)
        {
            FileList_browse.Items.Add(file);
        }

        // adds item to Files Listbox in metadata tab
        private void addFile_Metadata(string file)
        {
            FileList_metadata.Items.Add(file);
        }

        // adds item to Files Listbox in checkout tab
        private void addFile_Checkout(string file, string author, string desc, string status)
        {
            FileList_checkout.Items.Add(new FileDetails
            {
                fname_enabled = false,
                author_enabled = false,
                desc_enabled = false,
                cbox_enabled = false,
                status_enabled = false,
                FileName = file,
                Author = author,
                Description = desc,
                Status = status
            });
        }

        // adds item to Files Listbox in file tab
        private void addFile_File(string file)
        {
            FileList_file.Items.Add(file);
        }

        private void addFile_Checkin(string file,string author,string desc,string status)
        {

            FileList_checkin.Items.Add(new FileDetails {
                fname_enabled = false,
                author_enabled = false,
                desc_enabled = false,
                cbox_enabled = false,
                status_enabled=false,
                FileName = file,
                Author = author,
                Description = desc,
                Status = status });
            
           //  FileList_checkin.Items[FileList_checkin.Items.Count-1];
           // ip.IsEnabled = false;

        }


        // strip of name from first part of path in browse tab
        private string removeFirstDir_browse(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        // strip of name from first part of path in metadata tab
        private string removeFirstDir_metadata(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        // strip of name from first part of path in checkout tab
        private string removeFirstDir_checkout(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        // strip of name from first part of path in checkin tab
        private string removeFirstDir_checkin(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }
        // strip of name from first part of path in file tab
        private string removeFirstDir_file(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }








        // dispatcher to load files in Files ListBox for Browse tab
        private void DispatcherLoadGetFiles_Browse()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles_Browse();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("key"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile_Browse(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFilesbrowse", getFiles);
        }

        // dispatcher to load files in Directory ListBox for metadata tab
        private void DispatcherLoadGetDirs_Metadata()
        {

            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs_Metadata();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir_Metadata(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent_Metadata();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirsmetadata", getDirs);




        }

        // dispatcher to load files in Files ListBox for metadata tab
        private void DispatcherLoadGetFiles_Metadata()
        {

            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles_Metadata();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile_Metadata(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFilesmetadata", getFiles);




        }

        // dispatcher to load files in Directory ListBox for checkout tab
        private void DispatcherLoadGetDirs_Checkout()
        {

            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs_Checkout();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir_Checkout(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent_Checkout();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirscheckout", getDirs);




        }

        // dispatcher to load files in Files ListBox for checkout tab
        private void DispatcherLoadGetFiles_Checkout()
        {

            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles_Checkout();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        string temp = key;
                        int counter = Int32.Parse(temp.Replace("file", ""));
                        Action<string, int> doFile = (string file, int count) =>
                        {

                            addFile_Checkout(file, rcvMsg.value("author" + count), rcvMsg.value("description" + count), rcvMsg.value("status" + count));
                        };
                        Dispatcher.Invoke(doFile, enumer.Current.Value.ToString(), counter);
                    }
                }
            };
            addClientProc("getFilescheckout", getFiles);




        }

        // dispatcher to load files in Directory ListBox for checkin tab
        private void DispatcherLoadGetDirs_Checkin()
        {

            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs_Checkin();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir_Checkin(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent_Checkin();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirscheckin", getDirs);


        }

        // Dispatcher for getFiles Checkin Tab
        private void DispatcherLoadGetFiles_Checkin()
        {

            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles_Checkin();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        string temp = key;
                        int counter = Int32.Parse(temp.Replace("file", ""));
                        Action<string, int> doFile = (string file, int count) =>
                        {

                            addFile_Checkin(file, rcvMsg.value("author" + count), rcvMsg.value("description" + count), rcvMsg.value("status" + count));
                        };
                        Dispatcher.Invoke(doFile, enumer.Current.Value.ToString(), counter);
                    }
                }
            };
            addClientProc("getFilescheckin", getFiles);



        }



        // dispatcher to load files in Directory ListBox for file tab
        private void DispatcherLoadGetDirs_File()
        {

            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs_File();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir_File(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent_File();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirsfile", getDirs);


        }
        // dispatcher to load files in Files ListBox for file tab
        private void DispatcherLoadGetFiles_File()
        {

            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles_File();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile_File(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFilesfile", getFiles);




        }

        // Used to populate the Database Keys in Add Children popup Window
        private void Sentdbelements()
        {

            Action<CsMessage> sentelements = (CsMessage rcvMsg) =>
            {
                DBCore dbcore = new DBCore();
                List<string> keys = new List<string>();
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("key"))
                    {
                        DBElement tempelem = new DBElement();

                        string temp = key;
                        int counter = Int32.Parse(temp.Replace("key", ""));
                        string key_name = key + counter.ToString();
                        tempelem.KeyGetterSetter = enumer.Current.Value;
                        
                            List<String> children = new List<string>();
                            var enumer_child = rcvMsg.attributes.GetEnumerator();
                            while (enumer_child.MoveNext())
                            {
                                if (enumer_child.Current.Key.Contains("child" + counter.ToString()))
                                {
                                    children.Add(enumer_child.Current.Value);

                                }

                            }

                            tempelem.ChilderenGetterSetter = children;
                            dbcore.Set(counter,tempelem);
                    }

                }
                Action<DBCore,string> doFile = (DBCore database,string version) =>
                {

                    listChildren(database,version);

                };
                Dispatcher.Invoke(doFile, new Object[] { dbcore,rcvMsg.value("versionofcheckin") });
            };
            addClientProc("sentdbelements", sentelements);
        }





        // Used to populate the Database Keys in Add Children popup Window for automation
        private void SentdbelementsAuto()
        {

            Action<CsMessage> sentelementsauto = (CsMessage rcvMsg) =>
            {
                DBCore dbcore = new DBCore();
                List<string> keys = new List<string>();
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("key"))
                    {
                        DBElement tempelem = new DBElement();

                        string temp = key;
                        int counter = Int32.Parse(temp.Replace("key", ""));
                        string key_name = key + counter.ToString();
                        tempelem.KeyGetterSetter = enumer.Current.Value;

                        List<String> children = new List<string>();
                        // child exists for this element
                        var enumer_child = rcvMsg.attributes.GetEnumerator();
                        while (enumer_child.MoveNext())
                        {
                            // string check = enumer_child.Current.Key;
                            if (enumer_child.Current.Key.Contains("child" + counter.ToString()))
                            {
                                children.Add(enumer_child.Current.Value);

                            }

                        }

                        tempelem.ChilderenGetterSetter = children;
                        dbcore.Set(counter, tempelem);
                    }

                }
                Action<DBCore, string> doFile = (DBCore database, string version) =>
                {

                    listChildrenAuto(database, version);

                };
                Dispatcher.Invoke(doFile, new Object[] { dbcore, rcvMsg.value("versionofcheckin") });
            };
            addClientProc("sentdbelementsauto", sentelementsauto);
        }


        // Used to populate the Database Keys in Add Children popup Window for automation
        private void SentdbelementsAuto2()
        {
            Action<CsMessage> sentelementsauto2 = (CsMessage rcvMsg) =>
            {
                DBCore dbcore = new DBCore();
                List<string> keys = new List<string>();
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("key"))
                    {
                        DBElement tempelem = new DBElement();

                        string temp = key;
                        int counter = Int32.Parse(temp.Replace("key", ""));
                        string key_name = key + counter.ToString();
                        tempelem.KeyGetterSetter = enumer.Current.Value;

                        List<String> children = new List<string>();
                        // child exists for this element
                        var enumer_child = rcvMsg.attributes.GetEnumerator();

                        while (enumer_child.MoveNext())
                        {
                            // string check = enumer_child.Current.Key;
                            if (enumer_child.Current.Key.Contains("child" + counter.ToString()))
                            {
                                children.Add(enumer_child.Current.Value);

                            }

                        }
                        tempelem.ChilderenGetterSetter = children;
                        dbcore.Set(counter, tempelem);
                    }
                }
                Console.WriteLine("done iteration..");
                Action<DBCore, string> doFile = (DBCore database, string version) =>
                {

                    listChildrenAuto2(database, version);

                };
                Dispatcher.Invoke(doFile, new Object[] { dbcore, rcvMsg.value("versionofcheckin") });
            };
            addClientProc("sentdbelementsauto2", sentelementsauto2);
        }


        // handles popup for browse tab




        private void showPopUp_browse(string filename,string content)
        {
            display_Text_browse.Text = display_Text_browse.Text + "\n Popup file in new Window";
            PopUp p = new PopUp();
            p.popup_content.Text = content;
            p.Title = filename;
            p.Show();

        }
        // handles popup for metadata tab
        private void showPopUp_metadata(string filename, string content)
        {

            display_Text_metadata.Text = display_Text_metadata.Text + "\n Popup file in new Window";
            PopUp p = new PopUp();
            p.popup_content.Text = content;
            p.Title = filename;
            p.Show();

        }


        // dispatcher for receive file
        private void receiveFile()
        {
            Action<CsMessage> receivedfile = (CsMessage rcvMsg) =>
            {

                Action<CsMessage> dispInTextBox = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    addMsgContent_checkin(content);

                };

                Dispatcher.Invoke(dispInTextBox, new Object[] { rcvMsg });


            };

            addClientProc("gotfile",receivedfile);

        }

        // dispatcher for connect to server
        private void connectToServer()
        {

            Action<CsMessage> connect = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> dispInTextBox = (CsMessage msgcontent) =>
                  {
                      string content = rcvMsg.value("content");
                     // string final = msgcontent + "\n" + content;
                      addMsgContent_Connect(content);

                  };

                Dispatcher.Invoke(dispInTextBox, new Object[] {rcvMsg});



            };
            addClientProc("readytoconnect",connect);
        }

        // Haldler for single ownership fail case
        private void OwnershipFail()
        {

            Action<CsMessage> connect = (CsMessage rcvMsg) =>
            {
                Action<CsMessage> dispInTextBox = (CsMessage msgcontent) =>
                {
                    // MessageBoxResult result = MessageBox.Show("SIngle Ownership Policy Failure !! Author not matching", "CheckIn", MessageBoxButton.OK, MessageBoxImage.Error);
                    AutoClosingMessageBox.Show("SIngle Ownership Policy Failure !! Author not matching", "CheckIn", 2000);
                };

                Dispatcher.Invoke(dispInTextBox, new Object[] { rcvMsg });



            };
            addClientProc("singleownershipfail", connect);
        }






        // dispatcher for sent files for browse
        private void sentFileToClient()
        {

            Action<CsMessage> sentfileforbrowse = (CsMessage rcvMsg) =>
            {

                Action<CsMessage> dispInTextBox1 = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    if (msgcontent.value("tab") == "checkout")
                    {
                        addMsgContent_checkout(content);
                    }else if (msgcontent.value("tab") == "browse")
                    {
                        addMsgContent_browse(content);

                        string str_filename = rcvMsg.value("filename");
                        string text = System.IO.File.ReadAllText(SavePathForClient + "/" + str_filename);
                        //Console.WriteLine(text);

                        Action<string, string> dispInTextBox = (string fname, string contents) =>
                         {
                             showPopUp_browse(fname, contents);

                         };

                        Dispatcher.Invoke(dispInTextBox, new Object[] { str_filename, text });
                    }
                };

                Dispatcher.Invoke(dispInTextBox1, new Object[] { rcvMsg });
                
            };

            addClientProc("sentfiletoclient", sentfileforbrowse);

        }

        // dispatcher for sending files for browse
        private void sendingFileForBrowse()
        {

            Action<CsMessage> sendingfileforbrowse = (CsMessage rcvMsg) =>
            {

                Action<CsMessage> dispInTextBox = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    addMsgContent_browse(content);

                };

                Dispatcher.Invoke(dispInTextBox, new Object[] { rcvMsg });


            };



            addClientProc("sendingfiletobrowse", sendingfileforbrowse);
        }


        // dispatcher for sent metadata
        private void sentMetadata()
        {

            Action<CsMessage> sentfileformetadata = (CsMessage rcvMsg) =>
            {

                Action<CsMessage> dispInTextBox1 = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    addMsgContent_metadata(content);

                };

                Dispatcher.Invoke(dispInTextBox1, new Object[] { rcvMsg });



                string str_filename = rcvMsg.value("filename");
                // string text = System.IO.File.ReadAllText("../../../LocalClientRepository/" + str_filename);
                //Console.WriteLine(text);
                string text="";
                text = text +"\n"+ rcvMsg.value("author");
                text= text + "\n" + rcvMsg.value("date");
                text = text + "\n" + rcvMsg.value("language");


                Action<string, string> dispInTextBox = (string fname, string content) =>
                {
                    showPopUp_metadata(fname, content);

                };

                Dispatcher.Invoke(dispInTextBox, new Object[] { str_filename, text });




            };

            addClientProc("sentmetadata", sentfileformetadata);


        }




        // dispatcher for checkout done
        private void checkOutDone()
        {

          Action<CsMessage> checkOutDone = (CsMessage rcvMsg) => { 

            Action <CsMessage> dispInTextBox1 = (CsMessage msgcontent) =>
            {
                string content = rcvMsg.value("content");
                // string final = msgcontent + "\n" + content;
                addMsgContent_checkout(content);

            };

            Dispatcher.Invoke(dispInTextBox1, new Object[] { rcvMsg });

          };

        addClientProc("checkoutfile", checkOutDone);



        }


        // dispatcher for file acknowledge
        private void fileReceivedAckClient()
        {

            Action<CsMessage> fileAckReceived = (CsMessage rcvMsg) => {

                Action<CsMessage> dispInTextBox1 = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    addMsgContent_checkin(content);

                };

                Dispatcher.Invoke(dispInTextBox1, new Object[] { rcvMsg });

            };

            addClientProc("filereceivedack", fileAckReceived);



        }

        // dispatcher for view file
        private void viewFile()
        {

            Action<CsMessage> filetoview = (CsMessage rcvMsg) => {

                Action<CsMessage> dispInTextBox1 = (CsMessage msgcontent) =>
                {
                    string content = rcvMsg.value("content");
                    // string final = msgcontent + "\n" + content;
                    addMsgContent_file(content);

                };

                Dispatcher.Invoke(dispInTextBox1, new Object[] { rcvMsg });

            };

            addClientProc("filegiven", filetoview);

        }


       

        // initializes files and directories of repo in metadata tab
        private void initializeMetadataTab()
        {

            PathTextBlock_metadata.Text = "Storage";
            pathStack_metadata.Push("../Storage");

            //Console.WriteLine("browse tab selected");


            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirsmetadata");
            msg1.add("path", pathStack_metadata.Peek());
            translater.postMessage(msg1);

            msg1.remove("command");
            msg1.add("command", "getFilesmetadata");
            translater.postMessage(msg1);

        }

        // initializes files and directories of repo in  checkout tab
        private void initializeCheckoutTab()
        {
            PathTextBlock_checkout.Text = "Storage";
            pathStack_checkout.Push("../Storage");

            //Console.WriteLine("browse tab selected");


            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirscheckout");
            msg1.add("path", pathStack_checkout.Peek());
            translater.postMessage(msg1);

            msg1.remove("command");
            msg1.add("command", "getFilescheckout");
            translater.postMessage(msg1);


        }
        // initializes files and directories of repo in checkin tab
        private void initializeCheckinTab()
        {

            PathTextBlock_checkin.Text = "Storage";
            pathStack_checkin.Push("../Storage");

            //Console.WriteLine("browse tab selected");


            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirscheckin");
            msg1.add("path", pathStack_checkout.Peek());
            translater.postMessage(msg1);



        }

        // initializes files and directories of repo in file tab
        private void initializeFileTab()
        {

            PathTextBlock_file.Text = "Storage";
            pathStack_file.Push("../Storage");

            //Console.WriteLine("browse tab selected");


            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirsfile");
            msg1.add("path", pathStack_checkout.Peek());
            translater.postMessage(msg1);

            msg1.remove("command");
            msg1.add("command", "getFilesfile");
            translater.postMessage(msg1);

        }

        // handler to call the dispatchers created
        public void loadDispatcherClient()
        {
            connectToServer();
            receiveFile();
           // DispatcherLoadGetDirs_Browse();
            DispatcherLoadGetFiles_Browse();
            DispatcherLoadGetDirs_Metadata();
            DispatcherLoadGetFiles_Metadata();
            DispatcherLoadGetDirs_Checkout();
            DispatcherLoadGetFiles_Checkout();
            DispatcherLoadGetDirs_Checkin();
            DispatcherLoadGetFiles_Checkin();
            DispatcherLoadGetDirs_File();
            DispatcherLoadGetFiles_File();
            sendingFileForBrowse();
            sentFileToClient();
            fileReceivedAckClient();
            sentMetadata();
            checkOutDone();
            viewFile();
            Sentdbelements();
            OwnershipFail();
            SentdbelementsAuto();
            SentdbelementsAuto2();
        }

       
        // window on load event
        private void window_Load(object sender, RoutedEventArgs e)
        {

            AddChildrenTest = new Button();
            AddChildrenTest.Name = "child_add_button_temp";
            Console.WriteLine("STARTED...!!!");

            AddChildrenTest.Click += new RoutedEventHandler(add_children_clickauto);

            AddChildrenTest2 = new Button();
            AddChildrenTest2.Click += new RoutedEventHandler(add_children_clickauto2);



            disconnect_button.IsEnabled = false;
            browsetab.IsEnabled = false;
            checkintab.IsEnabled = false;
            checkouttab.IsEnabled = false;
            viewfiletab.IsEnabled = false;
            viewmetatab.IsEnabled = false;
            connecttab.IsEnabled = true;
            loadDispatcherClient();

            string[] args = Environment.GetCommandLineArgs();

            if (args.Length == 3)
            {

                secondGUI(args[2]);

            }
            else
            {
                 Test_1a();

            }






        }

        // test function to popup 2nd GUI
        public async void secondGUI(string portno)
        {
            await putDelay1();

            this.client_port.Text = portno;
            this.display_Text_connect.Text = this.display_Text_connect.Text + "\n Running Client 2 on port 8083";
            connect_Click(this,null);

            await putDelay1();
            tabControl.SelectedIndex = 1;
            await putDelay1();
            DirList_checkin.SelectedIndex = 2;
            onselectdir_checkin(this,null);
            await putDelay1();
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "Logger.h",
                Author = "Shripad",
                Category = "Airlines",
                Description = "Works at Air-India",
                fname_enabled = false,
                author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/Logger.h"),
                //Children_value = "Prasanna",
                Status = "Closed"
            });

            FileList_checkin.SelectedIndex = 0;
            await putDelay1();
            checkin_click(this, null);

        }

 

        // mouse click for files on browse tab
        private void onselectfile_browse(object sender, MouseButtonEventArgs e)
        {

           // string path = pathStack_browse.Peek();
            string filename = FileList_browse.SelectedItem.ToString();

            display_Text_browse.Text = display_Text_browse.Text + "Asking Server to browse " + filename;
             // create a send file request message to server


             CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));

            msg1.add("command", "sendtoclient");
            msg1.add("source", "client");
            msg1.add("destination", "repository");
            //msg1.add("path", path);
            msg1.add("filename", filename);
            translater.postMessage(msg1);

        }


        // test function to understand click events via code
        private void testconnect()
        {
            RoutedEventArgs e = new RoutedEventArgs();
            connect_Click(this, e);
             

        }
        // mouse click for Dirs on metadatatab
        private void onselectdir_metadata(object sender, MouseButtonEventArgs e)
        {

            string selectedDir = (string)DirList_metadata.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_metadata.Count > 1)  // don't pop off "Storage"
                    pathStack_metadata.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_metadata.Peek() + "/" + selectedDir;
                pathStack_metadata.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock_metadata.Text = removeFirstDir_metadata(pathStack_metadata.Peek());

            // build message to get dirs and post it

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirsmetadata");
            msg.add("path", pathStack_metadata.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFilesmetadata");
            translater.postMessage(msg);




        }
        // mouse click for files on metadata tab
        private void onselectfile_metadata(object sender, MouseButtonEventArgs e)
        {

            string path = pathStack_metadata.Peek();
            string filename = FileList_metadata.SelectedItem.ToString();

            //Console.WriteLine(path + " ...." + filename);
            display_Text_metadata.Text = display_Text_metadata.Text + "\n" + "Asking Server to send " + path + "/" + filename + " file";

            // create a send file request message to server

            
            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));

            msg1.add("command", "sendtoclientmetadata");
            msg1.add("source", "client");
            msg1.add("destination", "repository");
            msg1.add("path", path);
            msg1.add("filename", filename);
            translater.postMessage(msg1);

            


        }
        // mouse click for Dirs on checkout tab
        private void onselectdir_checkout(object sender, MouseButtonEventArgs e)
        {

            string selectedDir = (string)DirList_checkout.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_checkout.Count > 1)  // don't pop off "Storage"
                    pathStack_checkout.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_checkout.Peek() + "/" + selectedDir;
                pathStack_checkout.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock_checkout.Text = removeFirstDir_checkout(pathStack_checkout.Peek());

            // build message to get dirs and post it

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirscheckout");
            msg.add("path", pathStack_checkout.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFilescheckout");
            translater.postMessage(msg);





        }
        // mouse click for files on checkout tab
        private void onselectfile_checkout(object sender, MouseButtonEventArgs e)
        {
            string path = pathStack_checkout.Peek();
            //string filename = FileList_checkout.SelectedItem.ToString();

            var filename = FileList_checkout.SelectedItem as FileDetails;

            if (filename != null)
            {
                
                string str_filename = filename.FileName.ToString();
                //string fulllocalpath = filename.LocalPath.ToString();
                display_Text_checkout.Text = display_Text_checkout.Text + "\n" + "Checking out " + path + "/" + str_filename + " file";

                string test = PathTextBlock_checkout.Text;
                int index = test.IndexOf('/');
                string str_nmspc = "";
                if (index > 0)
                {
                    str_nmspc = test.Substring(index + 1); //Findng namespace name
                }

                CsMessage msg1 = new CsMessage();
                msg1.add("to", CsEndPoint.toString(serverEndPoint));
                msg1.add("from", CsEndPoint.toString(endPoint_));

                msg1.add("command", "checkout");
                msg1.add("source", "client");
                msg1.add("destination", "repository");
                msg1.add("path", path);
                msg1.add("filename", str_filename);
                msg1.add("namespace",str_nmspc);
                translater.postMessage(msg1);

            }

                //Console.WriteLine(path + " ...." + filename);
                

            // create a send file request message to server


            



        }

        // Opens a new File Dailog to add file for Checkin to Repository
        private void browse_file(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            //string path;
            if (openFileDialog.ShowDialog() == true)
            {
                string fullfilepath=openFileDialog.FileName;
                fullPathOfCheckin = fullfilepath;

                string test = fullfilepath;
                string filename = "";
                int index = test.LastIndexOf('\\'); //Finding File Name

                if (index > 0)
                {
                    filename = test.Substring(index + 1);
                }
                FileList_checkin.Items.Add(new FileDetails {
                    FileName = filename,
                    fname_enabled = false,
                    author_enabled = true,
                    desc_enabled = true,
                    cbox_enabled = true,
                    cat_enabled = true,
                    status_enabled = true,
                    LocalPath = fullfilepath,
                    //Children_value = "Prasanna",
                    Status = "Open" });

                FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;


                

                
            }
            

        }
        // mouse click for Dirs on checkin tab
        private void onselectdir_checkin(object sender, MouseButtonEventArgs e)
        {

            string selectedDir = (string)DirList_checkin.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                checkin_CreateFolder.IsEnabled = true;
                browse_button.IsEnabled = false;
                if (pathStack_checkin.Count > 1)  // don't pop off "Storage"
                    pathStack_checkin.Pop();
                else
                    return;
            }
            else
            {
                checkin_CreateFolder.IsEnabled = false;
                browse_button.IsEnabled = true;
                path = pathStack_checkin.Peek() + "/" + selectedDir;
                pathStack_checkin.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock_checkin.Text = removeFirstDir_checkin(pathStack_checkin.Peek());

            // build message to get dirs and post it

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirscheckin");
            msg.add("path", pathStack_checkin.Peek());
            translater.postMessage(msg);



        }
        // mouse click for files on checkin tab


       

        // on connect button click event
        private void connect_Click(object sender, RoutedEventArgs e)
        {

            connect_button.IsEnabled = false;
            disconnect_button.IsEnabled = true;

            browsetab.IsEnabled = true;
            checkintab.IsEnabled = true;
            checkouttab.IsEnabled = true;
            viewfiletab.IsEnabled = true;
            viewmetatab.IsEnabled = true;
            browse_button.IsEnabled = false;
            // start Comm
            endPoint_ = new CsEndPoint();
            //endPoint_.machineAddress = "localhost";
            endPoint_.machineAddress = client_addr.Text;
            endPoint_.port = Int32.Parse(client_port.Text);
            translater = new Translater();
            translater.listen(endPoint_);

            processMessages();

            serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = server_addr.Text;
            serverEndPoint.port = Int32.Parse(server_port.Text);

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "connect");

            display_Text_connect.Text = display_Text_connect.Text + "\n" + "Client Connecting to Server...";


            translater.postMessage(msg);
            //initializeBrowseTab();
            initializeMetadataTab();
            initializeCheckoutTab();
            initializeCheckinTab();
            initializeFileTab();


        }
        // on disconnect button click event
        private void disconnect_Click(object sender, RoutedEventArgs e)
        {

            browsetab.IsEnabled = false;
            checkintab.IsEnabled = false;
            checkouttab.IsEnabled = false;
            viewfiletab.IsEnabled = false;
            viewmetatab.IsEnabled = false;
            pathStack_browse.Clear();
            pathStack_checkin.Clear();
            pathStack_checkout.Clear();
            pathStack_metadata.Clear();
            pathStack_file.Clear();
           // PathTextBlock_browse.Text = String.Empty;
            PathTextBlock_checkin.Text= String.Empty;
            PathTextBlock_checkout.Text = String.Empty;
            PathTextBlock_metadata.Text = String.Empty;
            PathTextBlock_file.Text = String.Empty;
            DirList_checkin.Items.Clear();
           // DirList_browse.Items.Clear();
            DirList_checkout.Items.Clear();
            DirList_metadata.Items.Clear();
            DirList_file.Items.Clear();
            //FileList_browse.Items.Clear();
            FileList_checkin.Items.Clear();
            FileList_checkout.Items.Clear();
            FileList_metadata.Items.Clear();
            FileList_file.Items.Clear();

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(endPoint_));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "quitthread");
            translater.postMessage(msg);
            connect_button.IsEnabled = true;
            disconnect_button.IsEnabled = false;
            display_Text_connect.Text =display_Text_connect.Text + "\nDisconnected from Server";


        }


        // mouse click for Dirs on file tab
        private void onselectdir_file(object sender, MouseButtonEventArgs e)
        {

            string selectedDir = (string)DirList_file.SelectedItem;
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_file.Count > 1)  // don't pop off "Storage"
                    pathStack_file.Pop();
                else
                    return;
            }
            else
            {
                path = pathStack_file.Peek() + "/" + selectedDir;
                pathStack_file.Push(path);
            }
            // display path in Dir TextBlcok
            PathTextBlock_file.Text = removeFirstDir_file(pathStack_file.Peek());

            // build message to get dirs and post it

            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirsfile");
            msg.add("path", pathStack_file.Peek());
            translater.postMessage(msg);

            // build message to get files and post it
            msg.remove("command");
            msg.add("command", "getFilesfile");
            translater.postMessage(msg);

        }
        // mouse click for Files on file tab
        private void onselectfile_file(object sender, MouseButtonEventArgs e)
        {
            string path = pathStack_file.Peek();
            string filename = FileList_file.SelectedItem.ToString();

           // Console.WriteLine(path + " ...." + filename);
            display_Text_file.Text = display_Text_file.Text + "\n" + "Client wants to view " + path + "/" + filename + " file";

            // create a send file request message to server


            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));

            msg1.add("command", "viewfile");
            msg1.add("source", "client");
            msg1.add("destination", "repository");
            msg1.add("path", path);
            msg1.add("filename", filename);
            translater.postMessage(msg1);



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
        // delay functions 
        async Task putDelay3()
        {
            await Task.Delay(3000);
        }
        // delay functions 
        async Task putDelay4()
        {
            await Task.Delay(4000);
        }

        // test function to show automation
        public async void Testex1()
        {
            await putDelay1();
            this.Title = "PrasannaDodwad - Client 1 - 8082";
            string serverstorage=System.IO.Path.GetFullPath("../../../../Storage/");string serversendfile = System.IO.Path.GetFullPath("../../../../SendFilesRepo/");string serversavefile= System.IO.Path.GetFullPath("../../../../ServerRepository/");
            Console.WriteLine("Server Repository is at : "+serverstorage);
            Console.WriteLine("\n\nDemonstrating Requirement 3 : Multiple Tabs for Client");Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\n\nConnect Tab :");Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Client is on port number 8082 , Server on port number 8080");
            Console.WriteLine("After establishing connection,directories and corresponding files of Repository are loaded in Check In , Check Out ,\n Browse, View File && View Metadata tab");
            RoutedEventArgs routedEventArgObject = new RoutedEventArgs();
            connect_Click(this, routedEventArgObject);
            await putDelay2();
            tabControl.SelectedIndex = 3; 
            await putDelay1();
            Console.WriteLine("\n\nBrowse Tab :");Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\nBrowsing File IComm.h from Server Repository");
            Console.WriteLine("\nFile located in : " + serverstorage + " directory");
            Console.WriteLine("\nThis file is copied from " + serverstorage + " to " + serversendfile);
            Console.WriteLine("\nThen downloaded from " + serversendfile + " to " + System.IO.Path.GetFullPath(SavePathForClient) + " using Comm File Transfer, Then Browsed and Popuped in new Window");
           // FileList_browse.SelectedIndex = 2;onselectfile_browse(this,null);
            await putDelay2();
            this.Focus();await putDelay1();
            tabControl.SelectedIndex = 1;await putDelay1();
            Console.WriteLine("\n\nCheckin Tab :");Console.WriteLine("-------------------------------------------------------------");
            onselectfile_checkintest(this,null);
            await putDelay1();
            tabControl.SelectedIndex = 2;
            await putDelay1();
            FileList_checkout.SelectedIndex = 0;
            await putDelay1();
            Console.WriteLine("\n\nCheckout Tab :");Console.WriteLine("-------------------------------------------------------------");Console.WriteLine("\nHere a message is sent from Client to Server\nServer sends response back to Client");
            onselectfile_checkout(this,null);await putDelay2();
            tabControl.SelectedIndex = 4;await putDelay1();
            FileList_file.SelectedIndex = 1;
            await putDelay1();
            Console.WriteLine("\n\nView File Tab :");Console.WriteLine("-------------------------------------------------------------");Console.WriteLine("\nHere a message is sent from Client to Server\nServer sends response back to Client");
            onselectfile_file(this,null);
            await putDelay1();
            tabControl.SelectedIndex = 5;
            await putDelay1();
            FileList_metadata.SelectedIndex = 1;
            await putDelay1();
            Console.WriteLine("\n\nView MetaData Tab :");Console.WriteLine("-------------------------------------------------------------");Console.WriteLine("\nHere a message is sent from Client to Server\nServer sends response back to Client");
            onselectfile_metadata(this, null);
            await putDelay2();this.Focus();
            Console.WriteLine("\n\nNew Client PopUp :");Console.WriteLine("-------------------------------------------------------------");Console.WriteLine("New client is started on port 8083....\n");
        }

        // test function to show automation
        public async void Test_1a()
        {
            await putDelay1();
            this.Title = "PrasannaDodwad - Client 1 - 8082";
            string serverstorage = System.IO.Path.GetFullPath("../../../../Storage/"); string serversendfile = System.IO.Path.GetFullPath("../../../../SendFilesRepo/"); string serversavefile = System.IO.Path.GetFullPath("../../../../ServerRepository/");
            Console.WriteLine("Server Repository is at : " + serverstorage);
            Console.WriteLine("\n\nDemonstrating Requirement 3 : Multiple Tabs for Client"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\n\nConnect Tab :"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Client is on port number 8082 , Server on port number 8080");
            Console.WriteLine("After establishing connection,directories and corresponding files of Repository are loaded in Check In , Check Out ,\n Browse");
            RoutedEventArgs routedEventArgObject = new RoutedEventArgs();
            connect_Click(this, routedEventArgObject);
            Console.WriteLine("\n\nCheckIn Tab :"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\nAFTER EVERY CHECKIN , DATABASE IS SHOWED IN CONSOLE OF ServerPrototype.exe");
            Console.WriteLine("\n\n Test Case 1 : Circular Dependency"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Trying to checkin 'FileSystem::FileSystem.h', but dependent on 'CheckIn::CheckIn.cpp.1' \n");
            Console.WriteLine("But in Database, 'CheckIn::CheckIn.cpp.1' is 'dependent on FileSystem::FileSystem.h.1' , So Message is popup-uped 'Cannot checkin' ");
            await putDelay1();
            tabControl.SelectedIndex = 1;
            await putDelay1();
            DirList_checkin.SelectedIndex = 4;
            onselectdir_checkin(this, null);
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "FileSystem.h",
                Author = "Parag",
                fname_enabled = false,
                author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                status_enabled = true,
                LocalPath = "...df.dfd.",
                //Children_value = "Prasanna",
                Status = "Open"
            });
            FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;
            var obj = FileList_checkin.SelectedItem as FileDetails;
            await putDelay1();
            AddChildrenTest.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await putDelay4();
            await putDelay2();
            Test_1b();
        }

        // test function to show automation
        public async void Test_1b()
        {

            await putDelay1();
            Console.WriteLine("\n\n Test Case 2 : Single Ownership Policy"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Trying to checkin 'FileSystem::FileSystem.h', but Where Author is 'Abhay' \n");
            Console.WriteLine("But in Database, 'FileSystem::FileSystem.h' is Checked-In with Author 'Priya' , So single ownership failure ");
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "FileSystem.h",
                Author = "Abhay",fname_enabled = false,author_enabled = true,
                desc_enabled = true,cbox_enabled = true,
                status_enabled = true, LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/FileSystem.h"),
                Status = "Open"
            });
            FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;
            await putDelay1();
            checkin_click(this,null);
            FileList_checkin.Items.Clear();
            await putDelay4();
            Console.WriteLine("\n\n Test Case 3 : Checkin in Closed state, but dependent file is in Open state"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Trying to checkin 'FileManager::FileMgr.h' in closed state , but dependent on 'CheckIn::CheckIn.cpp.1' \n");
            Console.WriteLine("But in Database, 'CheckIn::CheckIn.cpp.1' is Checked-In with status 'closed' ,  so changing status of fresh checkin to 'closing' ");
            DirList_checkin.SelectedIndex = 0;
            onselectdir_checkin(this, null);
            await putDelay1();
            DirList_checkin.SelectedIndex = 1;
            onselectdir_checkin(this, null);
            await putDelay1();
            string temp = PathTextBlock_checkin.Text;
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "FileMgr.h",Author = "Abhijeet",
                Category = "FileMgr",Description = "Works at Zynga",
                fname_enabled = false,author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/FileMgr.h"),
                Status = "Closed"
            });
            FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;
            AddChildrenTest2.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));
            await putDelay4();
            await putDelay1();
            Test_1b1();
        }

        // test function to show automation
        public async void Test_1b1()
        {
            Console.WriteLine("\n\n Test Case 4 : Trying to 'Close' existing Database file currently in 'Open' state , but dependent file is in Open state"); Console.WriteLine("-------------------------------------------------------------------------------");
            Console.WriteLine("Trying to 'Close' 'CheckIn::CheckIn.cpp.1' , but dependent on 'FileSystem::FilesSystem.h.1' \n");
            Console.WriteLine("But in Database, 'FileSystem::FilesSystem.h.1' is Checked-In with status 'Open' ,  so changing status of fresh checkin to 'Closing' ");
            FileList_checkin.Items.Clear();await putDelay1();
            DirList_checkin.SelectedIndex = 0;onselectdir_checkin(this, null);await putDelay1();
            DirList_checkin.SelectedIndex = 3;onselectdir_checkin(this, null);await putDelay1();
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "CheckIn.cpp",
                Author = "Sumanth",Category = "CheckIn",
                Description = "Works at NVIDIA",Children_value = "FileSystem::FileSystem.h.1;",
                fname_enabled = false,author_enabled = true,
                desc_enabled = true,cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/CheckIn.cpp"),
                Status = "Closed"
            });
            FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;
            await putDelay1();checkin_click(this, null);await putDelay2();
            Console.WriteLine("\n\n Test Case 5 : Trying to 'Close' existing Database file currently in 'Open' state , but not dependent on anyone"); Console.WriteLine("-------------------------------------------------------------");            Console.WriteLine("Trying to 'Close' 'FileSystem::FilesSystem.h.1' , not dependent on anyone \n");Console.WriteLine("Changing its state to 'Closed' , and all dependent closing accordingly ");
            FileList_checkin.Items.Clear();
            await putDelay1();
            DirList_checkin.SelectedIndex = 0;onselectdir_checkin(this, null);await putDelay1();
            DirList_checkin.SelectedIndex = 4;onselectdir_checkin(this, null);
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "FileSystem.h",
                Author = "Prasanna",
                Category = "FileMgr",
                Description = "Studies At Syracuse",
                fname_enabled = false,
                author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/FileSystem.h"),
                Status = "Closed"
            });
            FileList_checkin.SelectedIndex = FileList_checkin.Items.Count - 1;
            await putDelay1();checkin_click(this, null);await putDelay2();
            DirList_checkin.SelectedIndex = 0;onselectdir_checkin(this, null);
            FileList_checkin.Items.Clear();
             Test_1d();
        }
        // test function to show automation
        public async void Test_1c()
        {
            await putDelay1();
            Console.WriteLine("\n\nBrowse Tab :"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\n\nTest Case 1 : Browse with no Parent"); Console.WriteLine("-------------------------------------------------------------");
            await putDelay1();
            tabControl.SelectedIndex = 3;
            await putDelay1();
            noparent_click(this,null);
            Console.WriteLine("\n\nTest Case 2 : Open File for Browse"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Double click on Key popups the file in New Window");
            FileList_browse.SelectedIndex = 1;
            await putDelay1();
            onselectfile_browse(this, null);
            await putDelay2();
            this.Focus();
            FileList_browse.Items.Clear();
            Console.WriteLine("\n\nTest Case 3 : Browse OR Filter"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Filter on Category : CheckOut OR  CheckIn");
            txt_browse_category.Text = "CheckOut;CheckIn";
            await putDelay1();
            browse_browseclick(this, null);
            await putDelay1();
            Console.WriteLine("\n\nTest Case 4 : Browse OR Filter Combined with AND Filter"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Filter on (Category = CheckOut OR  CheckIn) AND Version = 1");
            await putDelay2();
            txt_browse_version.Text = "1";
            await putDelay1();
            browse_browseclick(this, null);
            await putDelay2();
            Console.WriteLine("\n\nCheckOut Tab :"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("\n\nChecking Out 'CheckIn::CheckIn.cpp.1' and all its dependencies ");
            Console.WriteLine("All Checked Out Files are stored at : " + System.IO.Path.GetFullPath("../../../../LocalClientRepository") + " directory");
            tabControl.SelectedIndex = 2;
            await putDelay1();
            DirList_checkout.SelectedIndex = 3;
            onselectdir_checkout(this, null);
            await putDelay1();
            FileList_checkout.SelectedIndex = 0;onselectfile_checkout(this, null);
            Console.WriteLine("\n\nSecond Client :"); Console.WriteLine("-------------------------------------------------------------");Console.WriteLine("2nd Client is running on port 8083 . Demonstrating Checkin Functionality for 2nd client");
        }
        // test function to show automation
        public async void Test_1d()
        {
            Console.WriteLine("\n\n Test Case 5 : Multiple CheckIns"); Console.WriteLine("-------------------------------------------------------------");
            Console.WriteLine("Checking In more than one file (2 files here) together ");
            Console.WriteLine("(1) Multiple::First.h (2) Multiple::Second.h");
            await putDelay1();
            await putDelay1();
            DirList_checkin.SelectedIndex = 5;
            onselectdir_checkin(this,null);
            await putDelay1();
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "First.h",
                Author = "Abhijeet",
                Category = "FileMgr",
                Description = "Works at Zynga",
                fname_enabled = false,
                author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/First.h"),
                //Children_value = "Prasanna",
                Status = "Closed"
            });
            FileList_checkin.Items.Add(new FileDetails
            {
                FileName = "Second.h",
                Author = "Abhijeet",
                Category = "FileMgr",
                Description = "Works at Zynga",
                fname_enabled = false,
                author_enabled = true,
                desc_enabled = true,
                cbox_enabled = true,
                cat_enabled = true,
                status_enabled = true,
                LocalPath = System.IO.Path.GetFullPath("../../../../tempfiles/Second.h"),
                //Children_value = "Prasanna",
                Status = "Closed"
            });
            FileList_checkin.SelectAll();
            await putDelay1();
            checkin_click(this, null);
            await putDelay2();
            Test_1c();
        }




        // test function for chekin test case
        private void onselectfile_checkintest(object sender, MouseButtonEventArgs e)
        {
            string serversave = "../../../../ServerRepository/";
            
            FileList_checkin.Items.Add("App.xaml.cs");
            FileList_checkin.SelectedIndex = 0;

            string filename = (string)FileList_checkin.SelectedItem;
            string pathinrepo = pathStack_checkin.Peek();
            string pathinclient = "../../../../Client/";
            string combinedpath = pathinclient + filename;
            string fullpath=System.IO.Path.GetFullPath(combinedpath);
            Console.WriteLine("\nChecking in :"+fullpath);

            display_Text_checkin.Text = display_Text_checkin.Text + "\n Checking in " + filename + " in " + pathinrepo + " directory";

            Console.WriteLine("\nFirst file is copied from "+fullpath+" to "+System.IO.Path.GetFullPath(SendPathForClient + "/" + filename));
            System.IO.File.Copy(fullpath, SendPathForClient + "/" + filename, true);

            //// create new message to send file to server
            //// first copy the file to the temporary location for client
            //// then send file to server....
            Console.WriteLine("\nThen file is sent from "+ System.IO.Path.GetFullPath(SendPathForClient + "/" + filename)+" to " + System.IO.Path.GetFullPath(serversave) + " via Comm File Transfer");

            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint));
            msg1.add("from", CsEndPoint.toString(endPoint_));

            msg1.add("command", "sendtorepo");
            msg1.add("source", "client");
            msg1.add("destination", "repository");
            msg1.add("file", filename);
            translater.postMessage(msg1);

            


            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));

            msg2.add("command", "filesentack");
            msg2.add("source", "client");
            msg2.add("destination", "repository");

            translater.postMessage(msg2);




        }

        // onclose window event
        private async void window_close(object sender, System.ComponentModel.CancelEventArgs e)
        {

            if (disconnect_button.IsEnabled == true)
            {
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(endPoint_));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "quitthread");

                //Thread.Sleep(1000);

                translater.postMessage(msg);

            }
            await putDelay2();
            e.Cancel = false;

        }

        // Handler to add new folder to Repository
        private void checkin_folderClick(object sender, RoutedEventArgs e)
        {
            string foldername = "";
            inputDialog = new AddFolder("Enter Folder Name", "");
            if (inputDialog.ShowDialog() == true)
            {
                foldername = inputDialog.Answer;
                DirList_checkin.Items.Add(foldername);
                display_Text_checkin.Text = display_Text_checkin.Text+"\nClient -> Requester server to create namespace " + foldername;
                CsMessage msg2 = new CsMessage(); //Creating a new message
                msg2.add("to", CsEndPoint.toString(serverEndPoint));
                msg2.add("from", CsEndPoint.toString(endPoint_));
                msg2.add("command", "createfolder");
                msg2.add("fname", foldername);
                // msg2.add("tab", "checkin");
                translater.postMessage(msg2);

            }



        }

        // Handler event for File CheckIn
        private void checkin_click(object sender, RoutedEventArgs e)
        {
            foreach (var filename1 in FileList_checkin.SelectedItems)
            {
                var filename = filename1 as FileDetails;
                string author = filename.Author;string description = filename.Description;string category = filename.Category;
                if (author == "" || description == "" || category == "")
                {
                    MessageBoxResult result = MessageBox.Show("Author Or Description Or Category cannot be empty !!", "CheckIn", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    if (filename != null)
                    {
                        string str_filename = filename.FileName.ToString();string fulllocalpath = filename.LocalPath.ToString();string pathinrepo = pathStack_checkin.Peek();
                        display_Text_checkin.Text = display_Text_checkin.Text + "\n Checking in " + str_filename + " in " + pathinrepo + " directory";
                        System.IO.File.Copy(fulllocalpath, SendPathForClient + "/" + str_filename, true);
                        CsMessage msg1 = new CsMessage();
                        msg1.add("to", CsEndPoint.toString(serverEndPoint));
                        msg1.add("from", CsEndPoint.toString(endPoint_));
                        msg1.add("command", "sendtorepo");msg1.add("source", "client");
                        msg1.add("destination", "repository");msg1.add("file", str_filename);
                        translater.postMessage(msg1);
                        string test = PathTextBlock_checkin.Text;
                        int index = test.IndexOf('/');
                        string str_nmspc = "";
                        if (index > 0)
                        {
                            str_nmspc = test.Substring(index + 1); //Findng namespace name
                        }
                        CsMessage msg2 = new CsMessage();
                        msg2.add("to", CsEndPoint.toString(serverEndPoint));
                        msg2.add("from", CsEndPoint.toString(endPoint_));
                        msg2.add("filename", str_filename);msg2.add("command", "filesentack");
                        msg2.add("source", "client");msg2.add("destination", "repository");
                        msg2.add("value", fulllocalpath);msg2.add("namespace", str_nmspc);
                        msg2.add("status", filename.Status);msg2.add("category", filename.Category);
                        msg2.add("description", filename.Description);
                        if (filename.Children_value != null)
                        {
                            msg2.add("children", filename.Children_value);
                        }
                        msg2.add("author", filename.Author);
                        translater.postMessage(msg2);
                    }
                }
            }
        }

        // Constructs a message to retrieve Database Keys from Repository
        private void add_children_click(object sender, RoutedEventArgs e)
        {
            FileList_checkin.UnselectAll();
            object buttonDataContext = (sender as Button).DataContext;
            ListBoxItem lbi =(ListBoxItem) FileList_checkin.ItemContainerGenerator.ContainerFromItem(buttonDataContext);
            if (lbi.IsSelected == false)
            {
                lbi.IsSelected = !lbi.IsSelected;
            }

            var filename = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }



            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "senddblist");
            msg2.add("filename",file_name);
            msg2.add("namespace",str_nmspc);
            translater.postMessage(msg2);
        }



        // Constructs a message to retrieve Database Keys from Repository for automation
        private void add_children_clickauto(object sender,EventArgs e)
        {
            
            var filename = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }



            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "senddblistauto");
            msg2.add("filename", file_name);
            msg2.add("namespace", str_nmspc);
            translater.postMessage(msg2);
        }

        // Constructs a message to retrieve Database Keys from Repository for automation
        private void add_children_clickauto2(object sender, EventArgs e)
        {
         

            var filename = FileList_checkin.SelectedItem as FileDetails;
            string file_name = filename.FileName;
            string test = PathTextBlock_checkin.Text;
            int index = test.IndexOf('/');
            string str_nmspc = "";
            if (index > 0)
            {
                str_nmspc = test.Substring(index + 1); //Findng namespace name
            }



            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "senddblistauto2");
            msg2.add("filename", file_name);
            msg2.add("namespace", str_nmspc);
            translater.postMessage(msg2);
        }

        // Button Click for No Parent Browse
        private void noparent_click(object sender, RoutedEventArgs e)
        {
            FileList_browse.Items.Clear();

            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "browsenoparent");
            translater.postMessage(msg2);


        }

        // Constructs a message for multi browse option for a Repository
        private void browse_browseclick(object sender, RoutedEventArgs e)
        {
            FileList_browse.Items.Clear();
            CsMessage msg = new CsMessage();
            int flag = 0;
            string text;
            text = "Client -> Asking server for files having filters - ";
            if (txt_browse_category.Text != "")
            {
                text = text + "Category (" + txt_browse_category.Text + ") ";
                msg.add("category", txt_browse_category.Text);
                flag = 1;
            }
            if (txt_browse_filename.Text != "")
            {
                text = text + "Filename (" + txt_browse_filename.Text + ") ";
                msg.add("filename", txt_browse_filename.Text);
                flag = 1;
            }
            if (txt_browse_dependency.Text != "")
            {
                text = text + "Dependency (" + txt_browse_dependency.Text + ") ";
                msg.add("dependency", txt_browse_dependency.Text);
                flag = 1;
            }
            if (txt_browse_version.Text != "")
            {
                text = text + "Version (" + txt_browse_version.Text + ") ";
                msg.add("version", txt_browse_version.Text);
                flag = 1;
            }
            if (flag == 1)
            {
                display_Text_browse.Text = display_Text_browse.Text + "\n" + text + "\n";
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "multibrowse");
                translater.postMessage(msg); //posting it to client senders queue
            }
            else
            {
                MessageBoxResult result = MessageBox.Show("You have not selected any filter !!", "Browse..", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }

    // Item Template to define the Checkin Tab , Checkout Tab ListBox Template
    public class FileDetails
    {
       // public event PropertyChangedEventHandler PropertyChanged;
        public string Status { get; set; }
        public string Author { get; set; }
        
        public string Description { get; set; }
        public string FileName { get; set; }
        public string Category { get; set; }
        public string LocalPath { get; set; }
        public string ServerPath { get; set; }
        public string Children { get; set; }
        public bool CheckBoxSelected { get; set; }
        public bool fname_enabled { get; set; }
        public bool cbox_enabled { get; set; }
        public bool cat_enabled { get; set; }
        public bool author_enabled { get; set; }
        public bool desc_enabled { get; set; }
        public bool status_enabled { get; set; }
        public string Children_value { get; set; }

        public List<string> Status_List { get; set; }
        public FileDetails()
        {
            Status_List = new List<string>();
            
            Status_List.Add("Open");
            Status_List.Add("Closed");
            Status_List.Add("Closing");
            Author = "Author";
            Description = "Description";
            Category = "Category";
           // Category = "Cat1;Cat2";
            CheckBoxSelected = false;
        }
    }

    class ComboboxDisableConverter : IValueConverter
    {
        public object Convert(object value, Type targetType,
        object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return value;
            // You can add your custom logic here to disable combobox item
            if ((string)value == "Closing")
                return true;

            return false;
        }

        public object ConvertBack(object value, Type targetType,
        object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }


}
