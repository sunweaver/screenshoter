/* screenshoter.c */
/* Copyright (C) 2004 German Poo-Caaman~o <gpoo@ubiobio.cl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/* found at: http://www.calcifer.org/archivos/screenshoter/ */

#include <gdk/gdk.h>
#include <glib.h>
#include <popt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static GdkPixbuf*
take_screenshot (gint new_width, gint new_height)
{
	gint width, height;
	GdkPixbuf *screenshot = NULL;
	GdkPixbuf *thumbnail = NULL;
	GdkWindow *root_win = NULL;

	root_win = gdk_get_default_root_window();

	width = gdk_window_get_width(root_win);
	height = gdk_window_get_height(root_win);

	screenshot = gdk_pixbuf_get_from_window (gdk_get_default_root_window(),
	                                         0, 0,
	                                         width,
	                                         height
	);

	if (new_width != 0 && new_height <= 0) {
		new_height =
		    new_width * ((gfloat) height / (gfloat) width);
	}

	if (new_height != 0 && new_width <= 0) {
		new_width =
		    new_height * ((gfloat) width / (gfloat) height);
	}

	if (new_height <= 0 && new_width <= 0) {
		new_height = height;
		new_width = width;
	}

	thumbnail = gdk_pixbuf_scale_simple (screenshot,
					     new_width,
					     new_height,
					     GDK_INTERP_BILINEAR);
	return thumbnail;

}

void handle_timeout(int signum)
{
	g_printerr("timeout\n");
	exit(1);
}

int
main (int argc, char **argv)
{
	gchar c;		/* used for argument parsing */
	gchar *filename = NULL;
	gint width = 0;
	gint height = 0;
	poptContext optCon;	/* context for parsing command-line options */
	GdkPixbuf *screenshot = NULL;
	GError *error = NULL;
	gchar *buffer;
	gsize buffer_size;
	gboolean base64 = FALSE;
	gboolean std = FALSE;
	FILE *outfile;
	gchar *b64data;
	gchar *quality = NULL;
	gint quality_i = -1;
	gint timeout = 0;

	const struct poptOption optionsTable[] = {
		{"outfile", 'o', POPT_ARG_STRING, &filename, 0,
		 "FILE.jpg or - for stdout", "Filename.jpg"},
		{"width", 'w', POPT_ARG_INT, &width, 0,
		 "Width of the output image (to be resized)", "Width"},
		{"height", 'h', POPT_ARG_INT, &height, 0,
		 "Height of the output image (to be resized)", "Height"},
		{"base64", 'm', POPT_ARG_NONE, &base64, 0,
		 "base64 encode the image", NULL},
		{"quality", 'q', POPT_ARG_STRING, &quality, 0, "quality [0-100], default 85"},
		{"timeout", 't', POPT_ARG_INT, &timeout, 0,
		 "Timeout", "Seconds"},
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
	};

	optCon =
	    poptGetContext (NULL, argc, (const char **) argv, optionsTable,
			    0);
	poptSetOtherOptionHelp (optCon, "[options] -o Filename.jpg");

	if (argc < 2) {
		poptPrintUsage (optCon, stderr, 0);
		return 1;
	}

	while ((c = poptGetNextOpt (optCon)) >= 0) {
		switch (c) {
		case 'w':
			if (width <= 0) {
				width = 0;
			}
			break;
		case 'h':
			if (height <= 0) {
				height = 0;
			}
			break;
		}
	}

	if (c < -1) {
		/* an error occurred during option processing */
		g_printerr ("%s: %s\n",
			    poptBadOption (optCon, POPT_BADOPTION_NOALIAS),
			    poptStrerror (c));
		return 1;
	}

	if (!filename) {
		poptPrintUsage (optCon, stderr, 0);
		g_printerr ("\n\tmissing -o\n");
		return -1;
	} else if (!strcmp(filename, "-")) {
		std = TRUE; /* write to stdout */
	}

	if (quality) {
		quality_i = atoi(quality);
		if (quality_i < 0 || quality_i > 100) {
			poptPrintUsage (optCon, stderr, 0);
			g_printerr("%s", "\n\tquality must be in range [0-100]\n");
			return 1;
		}
	} else {
		quality = "85";
	}

	signal(SIGALRM, handle_timeout);
	if (timeout > 0) {
		alarm(timeout);
	}

	gdk_init (&argc, &argv);

	screenshot = take_screenshot (width, height);
	if (std || base64) {
		gdk_pixbuf_save_to_buffer(screenshot, &buffer, &buffer_size,
			"jpeg", &error, "quality", quality, NULL);
	} else {
		gdk_pixbuf_save (screenshot, filename, "jpeg", &error, "quality", quality, NULL);
		return 0;
	}

	if (error != NULL) {
		g_printerr ("%s\n", error->message);
		g_error_free (error);
		return 1;
	}
	if (base64) {
		b64data = g_base64_encode(buffer, buffer_size);
	}
	if (std) {
		outfile = stdout;
	} else {
		outfile = fopen(filename, "w");
		if (!outfile) {
			perror("failed to open file: fopen:");
			return 1;
		}
	}
	if (fwrite(base64 ? b64data : buffer, 1,
		base64 ? strlen(b64data) : buffer_size, outfile) < 1) {
		perror("fwrite");
		return 1;
	}
	if (!std)
		fclose(outfile);	

	poptFreeContext (optCon);

	return 0;
}
