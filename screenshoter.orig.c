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

#include <gdk/gdk.h>
#include <popt.h>
#include <stdlib.h>

static void
take_screenshot (const gchar * file, gint new_width, gint new_height)
{
	gint width, height;
	GdkPixbuf *screenshot = NULL;
	GdkPixbuf *thumbnail = NULL;
	GError *error = NULL;

	g_assert (file != NULL);

	width = gdk_screen_width ();
	height = gdk_screen_height ();

	screenshot = gdk_pixbuf_get_from_drawable (NULL,
						   gdk_get_default_root_window
						   (), NULL, 0, 0, 0, 0,
						   width, height);

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

	gdk_pixbuf_save (thumbnail, file, "png", &error, NULL);

	if (error != NULL) {
		g_printerr ("%s", error->message);
		g_error_free (error);
	}
}

int
main (int argc, char **argv)
{
	gchar c;		/* used for argument parsing */
	const gchar *filename;
	gint width = 0;
	gint height = 0;
	poptContext optCon;	/* context for parsing command-line options */

	const struct poptOption optionsTable[] = {
		{"width", 'w', POPT_ARG_INT, &width, 0,
		 "Width of the output image (to be resized)", "Width"},
		{"height", 'h', POPT_ARG_INT, &height, 0,
		 "Height of the output image (to be resized)", "Height"},
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
	};

	optCon =
	    poptGetContext (NULL, argc, (const char **) argv, optionsTable,
			    0);
	poptSetOtherOptionHelp (optCon, "[options] <filename>");

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
	filename = poptGetArg (optCon);
	if ((filename == NULL) || !(poptPeekArg (optCon) == NULL)) {
		poptPrintUsage (optCon, stderr, 0);
		g_printerr ("\n\tYou must to specify the output filename"
			    " such as screenshot.png\n");
		return -1;
	}

	if (c < -1) {
		/* an error occurred during option processing */
		g_printerr ("%s: %s\n",
			    poptBadOption (optCon, POPT_BADOPTION_NOALIAS),
			    poptStrerror (c));
		return 1;
	}

	gdk_init (&argc, &argv);

	take_screenshot (filename, width, height);

	poptFreeContext (optCon);

	return 0;
}
