#include <gtk/gtk.h>
#include <stdio.h>
#include <atmi.h>
#include <fml32.h>
#include <string.h>
#include "biblioProyecto.fml.h"

// Definición de la estructura de datos para los pagos
typedef struct {
    int numpago;
    char nomdes[256];
    char nomrem[256];
    char concepto[256];
    GtkWidget *num_entry_insert;
    GtkWidget *nomdes_entry_insert;
    GtkWidget *nomrem_entry_insert;
    GtkWidget *concepto_entry_insert;
    GtkWidget *output_label_insert;

    GtkWidget *num_entry_search;
    GtkWidget *output_label_search;
    GtkWidget *num_entry;
    GtkWidget *output_label;
} pagos;

// Función para enviar la solicitud de inserción al servidor
void enviar_solicitud_insercion(GtkWidget *widget, pagos *pgs) {
    long lvL_tamLongt;
    gchar msgbuf[1024];

    // Obtener los valores ingresados en los campos de entrada de la pestaña de inserción
    const gchar *num_pago = gtk_entry_get_text(GTK_ENTRY(pgs->num_entry_insert));
    const gchar *nom_des = gtk_entry_get_text(GTK_ENTRY(pgs->nomdes_entry_insert));
    const gchar *nom_rem = gtk_entry_get_text(GTK_ENTRY(pgs->nomrem_entry_insert));
    const gchar *concepto = gtk_entry_get_text(GTK_ENTRY(pgs->concepto_entry_insert));

    // Convertir los valores a los tipos necesarios
    pgs->numpago = atoi(num_pago);
    strncpy(pgs->nomdes, nom_des, sizeof(pgs->nomdes));
    strncpy(pgs->nomrem, nom_rem, sizeof(pgs->nomrem));
    strncpy(pgs->concepto, concepto, sizeof(pgs->concepto));

    // Conectar con el servidor Tuxedo
    if (tpinit((TPINIT *)NULL) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_insert), "Error en la conexión");
        return;
    }

    // Reservar espacio para el buffer FML
    FBFR32 *fbfr;
    if ((fbfr = (FBFR32 *)tpalloc("FML32", NULL, 1024)) == NULL) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_insert), "Error reservando espacio para el buffer FML");
        tpterm();
        return;
    }

    // Agregar los datos al buffer FML
    if (Fadd32(fbfr, NUMPAGO, (char *)&pgs->numpago, 0) < 0 ||
        Fadd32(fbfr, NOMDES, pgs->nomdes, 0) < 0 ||
        Fadd32(fbfr, NOMREM, pgs->nomrem, 0) < 0 ||
        Fadd32(fbfr, CONCEPTO, pgs->concepto, 0) < 0) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_insert), "Error insertando campos FML");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Invocar el servicio 'INSERT_PAGO_FML32'
    if (tpcall("INSERT_PAGO_FML32", (char *)fbfr, 0, (char **)&fbfr, &lvL_tamLongt, 0L) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_insert), "Error en la llamada al servicio");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Obtener y mostrar la respuesta del servidor
    FLDLEN32 flen = sizeof(msgbuf);
    Fget32(fbfr, OUTPUT, 0, (char *)msgbuf, &flen);
    gtk_label_set_text(GTK_LABEL(pgs->output_label_insert), (const gchar *)msgbuf);

    // Liberar el buffer y desconectar del servidor
    tpfree((char *)fbfr);
    tpterm();
}

// Función para enviar la solicitud de búsqueda al servidor
void enviar_solicitud_busqueda(GtkWidget *widget, pagos *pgs) {
    long lvL_tamLongt;
    gchar msgbuf[1024];

    // Obtener el número de pago ingresado en la pestaña de búsqueda
    const gchar *num_pago_str = gtk_entry_get_text(GTK_ENTRY(pgs->num_entry_search));
    pgs->numpago = atoi(num_pago_str);

    // Conectar con el servidor Tuxedo
    if (tpinit((TPINIT *)NULL) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_search), "Error en la conexión");
        return;
    }

    // Reservar espacio para el buffer FML
    FBFR32 *fbfr;
    if ((fbfr = (FBFR32 *)tpalloc("FML32", NULL, 1024)) == NULL) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_search), "Error reservando espacio para el buffer FML");
        tpterm();
        return;
    }

    // Agregar el número de pago al buffer FML
    if (Fadd32(fbfr, NUMPAGO, (char *)&pgs->numpago, 0) < 0) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_search), "Error insertando campo FML (NUMPAGO)");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Llamar al servicio 'SELECT_PAGO_FML32'
    if (tpcall("SELECT_PAGO_FML32", (char *)fbfr, 0, (char **)&fbfr, &lvL_tamLongt, 0L) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label_search), "Error en la llamada al servicio");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Obtener los resultados del buffer FML y mostrarlos en la interfaz
    gchar *output_text = g_strdup("Valores del registro:\n");
    FLDLEN32 flen;
    char *valor;

    if ((valor = (char *)Ffind32(fbfr, NOMREM, 0, &flen)) != NULL) {
        output_text = g_strconcat(output_text, "Nombre del remitente: ", valor, "\n", NULL);
    }
    if ((valor = (char *)Ffind32(fbfr, NOMDES, 0, &flen)) != NULL) {
        output_text = g_strconcat(output_text, "Nombre del destinatario: ", valor, "\n", NULL);
    }
    if ((valor = (char *)Ffind32(fbfr, CONCEPTO, 0, &flen)) != NULL) {
        output_text = g_strconcat(output_text, "Concepto de pago: ", valor, "\n", NULL);
    }
    gtk_label_set_text(GTK_LABEL(pgs->output_label_search), output_text);
    g_free(output_text);

    // Liberar el buffer y desconectar del servidor
    tpfree((char *)fbfr);
    tpterm();
}

// Función para enviar la solicitud de pago al servidor
void enviar_solicitud(GtkWidget *widget, pagos *pgs) {
    long lvL_tamLongt;
    // Obtener el concepto ingresado
    const gchar *concepto_str = gtk_entry_get_text(GTK_ENTRY(pgs->num_entry));
    strncpy(pgs->concepto, concepto_str, sizeof(pgs->concepto) - 1);

    // Conectar con el servidor Tuxedo
    if (tpinit((TPINIT *)NULL) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label), "Error en la conexión");
        return;
    }

    // Reservar espacio para el buffer FML
    FBFR32 *fbfr;
    if ((fbfr = (FBFR32 *)tpalloc("FML32", NULL, 1024)) == NULL) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label), "Error reservando espacio para el buffer FML");
        tpterm();
        return;
    }

    // Agregar el concepto al buffer FML
    if (Fadd32(fbfr, CONCEPTO, pgs->concepto, 0) < 0) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label), "Error insertando campo FML (CONCEPTO)");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Llamar al servicio 'SELECTALL_PAGO_FML32'
    if (tpcall("SELECTALL_PAGO_FML32", (char *)fbfr, 0, (char **)&fbfr, &lvL_tamLongt, 0L) == -1) {
        gtk_label_set_text(GTK_LABEL(pgs->output_label), "Error en la llamada al servicio");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Obtener los resultados del buffer FML y mostrarlos en la interfaz
    gchar *output_text = g_strdup("Valores del registro:\n");
    FLDLEN32 flen;
    char *valor;
    int num_registros = Foccur32(fbfr, NUMPAGO);
    int i;
    if (num_registros > 0) {
        for (i = 0; i < num_registros; ++i) {
            output_text = g_strconcat(output_text, "Nombre del remitente: ", Ffind32(fbfr, NOMREM, i, &flen), "\n", NULL);
            output_text = g_strconcat(output_text, "Nombre del destinatario: ", Ffind32(fbfr, NOMDES, i, &flen), "\n", NULL);
            output_text = g_strconcat(output_text, "Concepto de pago: ", Ffind32(fbfr, CONCEPTO, i, &flen), "\n", NULL);
            output_text = g_strconcat(output_text, "-------------------------\n", NULL);
        }
    } else {
        output_text = g_strdup("No se encontraron registros para el concepto especificado.");
    }
    gtk_label_set_text(GTK_LABEL(pgs->output_label), output_text);
    g_free(output_text);

    // Liberar el buffer y desconectar del servidor
    tpfree((char *)fbfr);
    tpterm();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *box_insert;
    GtkWidget *box_search;
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *button_insert;
    GtkWidget *button_search;
    GtkWidget *button;
    pagos pgs;

    gtk_init(&argc, &argv);

    // Crear la ventana principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Aplicación de Pagos");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Crear un notebook
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Pestaña de inserción
    box_insert = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_insert), 10);

    label = gtk_label_new("Insertar Pago");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box_insert, label);

    // Crear etiqueta y campo de entrada para el número de pago (Inserción)
    label = gtk_label_new("Número de pago:");
    gtk_box_pack_start(GTK_BOX(box_insert), label, FALSE, FALSE, 0);
    pgs.num_entry_insert = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_insert), pgs.num_entry_insert, FALSE, FALSE, 0);

    // Crear etiqueta y campo de entrada para el nombre del destinatario (Inserción)
    label = gtk_label_new("Nombre del destinatario:");
    gtk_box_pack_start(GTK_BOX(box_insert), label, FALSE, FALSE, 0);
    pgs.nomdes_entry_insert = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_insert), pgs.nomdes_entry_insert, FALSE, FALSE, 0);

    // Crear etiqueta y campo de entrada para el nombre del remitente (Inserción)
    label = gtk_label_new("Nombre del remitente:");
    gtk_box_pack_start(GTK_BOX(box_insert), label, FALSE, FALSE, 0);
    pgs.nomrem_entry_insert = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_insert), pgs.nomrem_entry_insert, FALSE, FALSE, 0);

    // Crear etiqueta y campo de entrada para el concepto de pago (Inserción)
    label = gtk_label_new("Concepto de pago:");
    gtk_box_pack_start(GTK_BOX(box_insert), label, FALSE, FALSE, 0);
    pgs.concepto_entry_insert = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_insert), pgs.concepto_entry_insert, FALSE, FALSE, 0);

    // Crear un botón para enviar la solicitud de inserción
    button_insert = gtk_button_new_with_label("Insertar Pago");
    g_signal_connect(button_insert, "clicked", G_CALLBACK(enviar_solicitud_insercion), &pgs);
    gtk_box_pack_start(GTK_BOX(box_insert), button_insert, FALSE, FALSE, 0);

    // Crear una etiqueta para mostrar los resultados (Inserción)
    pgs.output_label_insert = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(box_insert), pgs.output_label_insert, FALSE, FALSE, 0);

    // Pestaña de búsqueda
    box_search = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_search), 10);

    label = gtk_label_new("Buscar Pago");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box_search, label);

    // Crear etiqueta y campo de entrada para el número de pago (Búsqueda)
    label = gtk_label_new("Número de pago:");
    gtk_box_pack_start(GTK_BOX(box_search), label, FALSE, FALSE, 0);
    pgs.num_entry_search = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_search), pgs.num_entry_search, FALSE, FALSE, 0);

    // Crear un botón para enviar la solicitud de búsqueda
    button_search = gtk_button_new_with_label("Buscar Pago");
    g_signal_connect(button_search, "clicked", G_CALLBACK(enviar_solicitud_busqueda), &pgs);
    gtk_box_pack_start(GTK_BOX(box_search), button_search, FALSE, FALSE, 0);

    // Crear una etiqueta para mostrar los resultados (Búsqueda)
    pgs.output_label_search = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(box_search), pgs.output_label_search, FALSE, FALSE, 0);

    // Pestaña de solicitud
    GtkWidget *box_solicitud = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box_solicitud), 10);

    label = gtk_label_new("Solicitud de Pago");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box_solicitud, label);

    // Crear etiqueta y campo de entrada para el concepto de pago (Solicitud)
    label = gtk_label_new("Concepto de pago:");
    gtk_box_pack_start(GTK_BOX(box_solicitud), label, FALSE, FALSE, 0);
    pgs.num_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box_solicitud), pgs.num_entry, FALSE, FALSE, 0);

    // Crear un botón para enviar la solicitud de pago
    button = gtk_button_new_with_label("Enviar Solicitud");
    g_signal_connect(button, "clicked", G_CALLBACK(enviar_solicitud), &pgs);
    gtk_box_pack_start(GTK_BOX(box_solicitud), button, FALSE, FALSE, 0);

    // Crear una etiqueta para mostrar los resultados de la solicitud
    pgs.output_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(box_solicitud), pgs.output_label, FALSE, FALSE, 0);

    // Mostrar la ventana y empezar el bucle de eventos
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
