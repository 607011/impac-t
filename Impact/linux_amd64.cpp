#include "linux_amd64.h"
#include <gtk/gtk.h>

namespace Impact {
   
   namespace Linux_AMD64 {

      struct fch_result {
	  gint response;
	  std::string* filename;
      };

      static gboolean fch_dialog(gpointer user_data)
      {
	  struct fch_result *result = (struct fch_result *) user_data;
	  GtkWidget *dialog =
		  gtk_file_chooser_dialog_new ("Open File",
					       NULL,
					       GTK_FILE_CHOOSER_ACTION_OPEN,
					       "_Cancel",
					       GTK_RESPONSE_CANCEL,
					       "_Open",
					       GTK_RESPONSE_ACCEPT,
					       NULL);
	  result->response = gtk_dialog_run (GTK_DIALOG(dialog));
	  char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	  *(result->filename) = filename;
	  g_free(filename);
	  gtk_widget_destroy(dialog);
	  gtk_main_quit();  // terminate the gtk_main loop called from caller
	  return FALSE;
      }

      bool choose_file(std::string& filename) {
	  struct fch_result data;
	  data.filename = &filename;
	  g_idle_add(fch_dialog, &data);
	  gtk_main();
	  return (data.response == GTK_RESPONSE_ACCEPT);
      };

   }
}
