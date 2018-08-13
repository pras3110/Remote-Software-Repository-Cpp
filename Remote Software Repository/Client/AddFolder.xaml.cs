

///////////////////////////////////////////////////////////////////////////////////////
// AddFolder.xaml.cs -Demonstrates Add Folder to Repository Functionality            //
// Author: Prasanna Dodwad, pdodwad@syr.edu                                          //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                     //
// Environment: C# console                                                           //             
///////////////////////////////////////////////////////////////////////////////////////


/*
 * 
 *   Package Operations:
 *   -------------------
 *   This package defines the Add Folder functionality for the project4. Its functionality is to:
 *   
 *   - Allow Client to add Folder Structure for a particular CheckIn in Repository
 *   
 *    * 
* Required Files:
* ---------------
* AddFolder.xaml , AddFolder.xaml.cs
* 
* 
* 
* Maintenance History:
* --------------------
* ver 1.0 : 1st May 2018


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
    /// Interaction logic for AddFolder.xaml
    /// </summary>
    public partial class AddFolder : Window
    {
        public AddFolder(string question, string defaultAnswer = "")
        {
            InitializeComponent();
            lblQuestion.Content = question;
            txtAnswer.Text = defaultAnswer;
        }
        //Closes Add Folder Window
        private void btnDialogOk_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }

        //Content on window is rendered
        private void Window_ContentRendered(object sender, EventArgs e)
        {
            txtAnswer.SelectAll();
            txtAnswer.Focus();
        }

        public string Answer
        {
            get { return txtAnswer.Text; }
        }
    }
}
