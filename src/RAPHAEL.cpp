/*
 * This file is part of ReporteRs.
 * Copyright (c) 2014, David Gohel All rights reserved.
 *
 * ReporteRs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ReporteRs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ReporteRs.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <string.h>
#include <stdio.h>
#include <math.h>

#include <locale.h>
#include <R_ext/Riconv.h>
#include <errno.h>
#include "RAPHAEL.h"

#define R_USE_PROTOTYPES 1

#include "datastruct.h"
#include "colors.h"
#include "raphael_utils.h"
#include "common.h"
#include "dml_utils.h"
#include "utils.h"


static Rboolean RAPHAELDeviceDriver(pDevDesc dev, const char* filename, double* width,
		double* height, double* offx, double* offy, double ps, int nbplots,
		const char* fontname, int canvas_id, SEXP env) {

	DOCDesc *rd;
	rd = (DOCDesc *) malloc(sizeof(DOCDesc));

	FontInfo *fi = (FontInfo *) malloc(sizeof(FontInfo));
	fi->isinit=0;
	fi->fontsize=(int)ps;
	rd->fi = fi;

	ElementTracer *elt_tracer = (ElementTracer *) malloc(sizeof(ElementTracer));
	elt_tracer->on = 0;
	elt_tracer->isinit = 0;
	rd->elt_tracer = elt_tracer;

	rd->canvas_id = canvas_id;

	rd->filename = strdup(filename);
	rd->fontname = strdup(fontname);
	rd->id = 0;
	rd->pageNumber = 0;
	rd->offx = offx[0];
	rd->offy = offy[0];
	rd->extx = width[0];
	rd->exty = height[0];
	rd->maxplot = nbplots;
	rd->x = offx;
	rd->y = offy;
	rd->width = width;
	rd->height = height;
	rd->fontface = 1;
	rd->fontsize = (int) ps;
	rd->env=env;
	//
	//  Device functions
	//
	dev->deviceSpecific = rd;
	dev->activate = RAPHAEL_activate;
	dev->close = RAPHAEL_Close;
	dev->size = RAPHAEL_Size;
	dev->newPage = RAPHAEL_NewPage;
	dev->clip = RAPHAEL_Clip;
	dev->strWidth = RAPHAEL_StrWidth;
	dev->strWidthUTF8 = RAPHAEL_StrWidthUTF8;
	dev->text = RAPHAEL_Text;
	dev->textUTF8 = RAPHAEL_TextUTF8;
	dev->rect = RAPHAEL_Rect;
	dev->circle = RAPHAEL_Circle;
	dev->line = RAPHAEL_Line;
	dev->polyline = RAPHAEL_Polyline;
	dev->polygon = RAPHAEL_Polygon;
	dev->metricInfo = RAPHAEL_MetricInfo;
	dev->hasTextUTF8 = (Rboolean) TRUE;
	dev->wantSymbolUTF8 = (Rboolean) TRUE;
	dev->useRotatedTextInContour = (Rboolean) FALSE;
	/*
	 * Initial graphical settings
	 */
	dev->startfont = 1;
	dev->startps = ps;
	dev->startcol = R_RGB(0, 0, 0);
	dev->startfill = R_TRANWHITE;
	dev->startlty = LTY_SOLID;
	dev->startgamma = 1;


	/*
	 * Device physical characteristics
	 */

	dev->left = 0;
	dev->right = width[0];
	dev->bottom = height[0];
	dev->top = 0;

	dev->clipLeft = 0;
	dev->clipRight = width[0];
	dev->clipBottom = height[0];
	dev->clipTop = 0;

	rd->clippedx0 = dev->clipLeft;
	rd->clippedy0 = dev->clipTop;
	rd->clippedx1 = dev->clipRight;
	rd->clippedy1 = dev->clipBottom;

	dev->cra[0] = 0.9 * ps;
	dev->cra[1] = 1.2 * ps;
	dev->xCharOffset = 0.4900;
	dev->yCharOffset = 0.3333;
	//dev->yLineBias = 0.2;
	dev->ipr[0] = 1.0 / 72.2;
	dev->ipr[1] = 1.0 / 72.2;
	/*
	 * Device capabilities
	 */
	dev->canClip = (Rboolean) TRUE;
	dev->canHAdj = 2;
	dev->canChangeGamma = (Rboolean) FALSE;
	dev->displayListOn = (Rboolean) FALSE;

	dev->haveTransparency = 2;
	dev->haveTransparentBg = 3;

	return (Rboolean) TRUE;
}


void GE_RAPHAELDevice(const char* filename, double* width, double* height, double* offx,
		double* offy, double ps, int nbplots, const char* fontfamily, int canvas_id, SEXP env) {
	pDevDesc dev = NULL;
	pGEDevDesc dd;
	R_GE_checkVersionOrDie (R_GE_version);
	R_CheckDeviceAvailable();

	if (!(dev = (pDevDesc) calloc(1, sizeof(DevDesc))))
		Rf_error("unable to start RAPHAEL device");
	if (!RAPHAELDeviceDriver(dev, filename, width, height, offx, offy, ps, nbplots,
			fontfamily, canvas_id, env)) {
		free(dev);
		Rf_error("unable to start RAPHAEL device");
	}

	dd = GEcreateDevDesc(dev);
	GEaddDevice2(dd, "RAPHAEL");

}

static void RAPHAEL_activate(pDevDesc dev) {
}


void RAPHAEL_SetLineSpec(pDevDesc dev, R_GE_gcontext *gc, int idx) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");

	float alpha =  R_ALPHA(gc->col)/255.0;

	fprintf(pd->dmlFilePointer, "elt_%d.attr({", idx);

	if (gc->lty > -1 && gc->lwd > 0.0 && alpha > 0) {
		fprintf(pd->dmlFilePointer, "'stroke': \"#%s\"", RGBHexValue(gc->col) );
		fprintf(pd->dmlFilePointer, ", 'stroke-opacity': \"%.3f\"", alpha );

		fprintf(pd->dmlFilePointer, ", 'stroke-width': %.3f", gc->lwd );

		switch (gc->lty) {
		case LTY_BLANK:
			break;
		case LTY_SOLID:
			break;
		case LTY_DOTTED:
			fputs(", 'stroke-dasharray': \".\"", pd->dmlFilePointer );
			break;
		case LTY_DASHED:
			fputs(", 'stroke-dasharray': \"-\"", pd->dmlFilePointer );
			break;
		case LTY_LONGDASH:
			fputs(", 'stroke-dasharray': \"--\"", pd->dmlFilePointer );
			break;
		case LTY_DOTDASH:
			fputs(", 'stroke-dasharray': \"-.\"", pd->dmlFilePointer );
			break;
		case LTY_TWODASH:
			fputs(", 'stroke-dasharray': \"--\"", pd->dmlFilePointer );
			break;
		default:
			break;
		}

		switch (gc->ljoin) {
		case GE_ROUND_JOIN: //round
			fputs(", 'stroke-linejoin': \"round\"", pd->dmlFilePointer );
			break;
		case GE_MITRE_JOIN: //mitre
			fputs(", 'stroke-linejoin': \"miter\"", pd->dmlFilePointer );
			break;
		case GE_BEVEL_JOIN: //bevel
			fputs(", 'stroke-linejoin': \"bevel\"", pd->dmlFilePointer );
			break;
		default:
			fputs(", 'stroke-linejoin': \"round\"", pd->dmlFilePointer );
			break;
		}
	} else {
		fputs("'stroke-width': 0", pd->dmlFilePointer );
	}
	fputs("});\n", pd->dmlFilePointer );
	setlocale(LC_NUMERIC, saved_locale);

}

void RAPHAEL_SetFillColor(pDevDesc dev, R_GE_gcontext *gc, int idx) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");
	fprintf(pd->dmlFilePointer, "elt_%d.attr({", idx);

	float alpha =  R_ALPHA(gc->fill)/255.0;
	if (alpha > 0) {
		fprintf(pd->dmlFilePointer, "'fill': \"#%s\"", RGBHexValue(gc->fill) );
		fprintf(pd->dmlFilePointer, ",'fill-opacity': \"%.3f\"", alpha );
	}
	fputs("});\n", pd->dmlFilePointer );
	setlocale(LC_NUMERIC, saved_locale);

}

void RAPHAEL_SetFontSpec(pDevDesc dev, R_GE_gcontext *gc, int idx) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");

	float alpha =  R_ALPHA(gc->col)/255.0;
	double fontsize = getFontSize(gc->cex, gc->ps);


	if ( gc->cex > 0.0 && alpha > 0 ) {
		fprintf(pd->dmlFilePointer, "elt_%d.attr({", idx);
		fprintf(pd->dmlFilePointer, "'fill': \"#%s\"", RGBHexValue(gc->col) );
		fprintf(pd->dmlFilePointer, ", 'fill-opacity': \"%.3f\"", alpha );
		fprintf(pd->dmlFilePointer, ", 'font-family': \"%s\"", pd->fi->fontname );
		fprintf(pd->dmlFilePointer, ", 'font-size': \"%.0f\"", fontsize );
		if (gc->fontface == 2) {
			fputs(", 'font-weight': \"bold\"", pd->dmlFilePointer );
		} else if (gc->fontface == 3) {
			fputs(", 'font-style':\"italic\"", pd->dmlFilePointer );
		} else if (gc->fontface == 4) {
			fputs(", 'font-weight': \"bold\", 'font-style':\"italic\"", pd->dmlFilePointer );
		}
		fputs("});\n", pd->dmlFilePointer );


	}

	setlocale(LC_NUMERIC, saved_locale);

}

static void RAPHAEL_Circle(double x, double y, double r, const pGEcontext gc,
		pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int idx = get_and_increment_idx(dev);
	register_element( dev);

	fprintf(pd->dmlFilePointer,
			"var elt_%d = %s.circle(%.5f, %.5f, %.5f);\n", idx, pd->objectname, x, y, r);
	RAPHAEL_SetLineSpec(dev, gc, idx);
	RAPHAEL_SetFillColor(dev, gc, idx);

	fflush(pd->dmlFilePointer);
}

static void RAPHAEL_Line(double x1, double y1, double x2, double y2,
		const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;

	DOC_ClipLine(x1, y1, x2, y2, dev);
	x1 = pd->clippedx0;y1 = pd->clippedy0;
	x2 = pd->clippedx1;y2 = pd->clippedy1;

	if (gc->lty > -1 && gc->lwd > 0.0 ){



		int idx = get_and_increment_idx(dev);
		register_element( dev);
		fprintf(pd->dmlFilePointer, "var elt_%d = %s.path(\"", idx, pd->objectname );
		fprintf(pd->dmlFilePointer, "M %.5f %.5f", x1, y1);
		fprintf(pd->dmlFilePointer, "L %.5f %.5f", x2, y2);
		fputs("\");\n", pd->dmlFilePointer );

		RAPHAEL_SetLineSpec(dev, gc, idx);

		fflush(pd->dmlFilePointer);
	}

	//return;
}

static void RAPHAEL_Polyline(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int i;
	for (i = 1; i < n; i++) {
		DOC_ClipLine(x[i-1], y[i-1], x[i], y[i], dev);
		if( i < 2 ){
			x[i-1] = pd->clippedx0;
			y[i-1] = pd->clippedy0;
		}
		x[i] = pd->clippedx1;
		y[i] = pd->clippedy1;
	}

	if (gc->lty > -1 && gc->lwd > 0.0 ){

		int idx = get_and_increment_idx(dev);
		register_element( dev);
		int i;
		fprintf(pd->dmlFilePointer, "var elt_%d = %s.path(\"", idx, pd->objectname );
		fprintf(pd->dmlFilePointer, "M %.5f %.5f", x[0], y[0]);

		for (i = 1; i < n; i++) {
			fprintf(pd->dmlFilePointer, "L %.5f %.5f", x[i], y[i]);
		}
		fputs("\");\n", pd->dmlFilePointer );


		RAPHAEL_SetLineSpec(dev, gc, idx);

		fflush(pd->dmlFilePointer);
	}
}

static void RAPHAEL_Polygon(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int i;
	for (i = 1; i < n; i++) {
		DOC_ClipLine(x[i-1], y[i-1], x[i], y[i], dev);
		if( i < 2 ){
			x[i-1] = pd->clippedx0;
			y[i-1] = pd->clippedy0;
		}
		x[i] = pd->clippedx1;
		y[i] = pd->clippedy1;
	}

	int idx = get_and_increment_idx(dev);
	register_element( dev);

	fprintf(pd->dmlFilePointer, "var elt_%d = %s.path(\"", idx, pd->objectname );
	fprintf(pd->dmlFilePointer, "M %.5f %.5f", x[0], y[0]);

	for (i = 1; i < n; i++) {
		fprintf(pd->dmlFilePointer, "L %.5f %.5f", x[i], y[i]);
	}

	fputs("Z\");\n", pd->dmlFilePointer );

	RAPHAEL_SetLineSpec(dev, gc, idx);
	RAPHAEL_SetFillColor(dev, gc, idx);

	fflush(pd->dmlFilePointer);

}

static void RAPHAEL_Rect(double x0, double y0, double x1, double y1,
		const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;

	DOC_ClipRect(x0, y0, x1, y1, dev);
	x0 = pd->clippedx0;y0 = pd->clippedy0;
	x1 = pd->clippedx1;y1 = pd->clippedy1;

	int idx = get_and_increment_idx(dev);
	register_element( dev );
	double temp;
	if( y1 < y0 ){
		temp = y1;
		y1 = y0;
		y0 = temp;
	}
	if( x1 < x0 ){
		temp = x1;
		x1 = x0;
		x0 = temp;
	}

	fprintf(pd->dmlFilePointer, "var elt_%d = %s.rect(", idx, pd->objectname );
	fprintf(pd->dmlFilePointer, "%.5f,%.5f", x0, y0);
	fprintf(pd->dmlFilePointer, ",%.5f,%.5f", x1-x0, y1-y0);
	fputs(");\n", pd->dmlFilePointer );

	RAPHAEL_SetLineSpec(dev, gc, idx);
	RAPHAEL_SetFillColor(dev, gc, idx);

	fflush(pd->dmlFilePointer);

}
void raphael_text(const char *str, DOCDesc *pd){
    for( ; *str ; str++)
	switch(*str) {
	case '"':
		fprintf(pd->dmlFilePointer, "\\\"");
	    break;

	case '\n':
		fprintf(pd->dmlFilePointer, "\\\n");
	    break;

	default:
	    fputc(*str, pd->dmlFilePointer);
	    break;
	}
}

void raphael_textUTF8(const char *str, DOCDesc *pd){
	unsigned char *p;
	p = (unsigned char *) str;
	int val, val1, val2, val3, val4;
	while(*p){
		val = *(p++);
		if( val < 128 ){ /* ASCII */
			switch(val) {
				case '"':
					fprintf(pd->dmlFilePointer, "\\\"");
					break;

				case '\n':
					fprintf(pd->dmlFilePointer, "\\\n");
					break;

				default:
					fprintf(pd->dmlFilePointer, "%c", val);
					break;
			}

		} else if( val > 240 ){ /* 4 octets*/
			val1 = (val - 240) * 65536;
			val = *(p++);
			val2 = (val - 128) * 4096;
			val = *(p++);
			val3 = (val - 128) * 64;
			val = *(p++);
			val4 = val - 128;
			val = val1 + val2 + val3 + val4;

			char byte1 = 0xf0 | ((val & 0x1C0000) >> 16);
			char byte2 = 0x80 | ((val & 0x3F000)  >> 12);
			char byte3 = 0x80 | ((val & 0xFC0) >> 6);
			char byte4 = 0x80 | (val & 0x3f);
			fprintf(pd->dmlFilePointer, "%c%c%c%c", byte1, byte2, byte3, byte4);
		} else {
			if( val >= 224 ){ /* 3 octets : 224 = 128+64+32 */
				val1 = (val - 224) * 4096;
				val = *(p++);
				val2 = (val-128) * 64;
				val = *(p++);
				val3 = (val-128);
				val = val1 + val2 + val3;
				char byte1 = 0xe0 | ((val & 0xf000) >> 12);
				char byte2 = 0x80 | ((val & 0xfc0)  >> 6);
				char byte3 = 0x80 | (val & 0x3f);
				fprintf(pd->dmlFilePointer, "%c%c%c", byte1, byte2, byte3);
			} else { /* 2 octets : >192 = 128+64 */
				val1 = (val - 192) * 64;
				val = *(p++);
				val2 = val-128;
				val = val1 + val2;
				char byte1 = 0xc0 | ((val & 0x7c0) >> 6);
				char byte2 = 0x80 | (val & 0x3f);
				fprintf(pd->dmlFilePointer, "%c%c", byte1, byte2);
			}

		}
	}
}


static void RAPHAEL_TextUTF8(double x, double y, const char *str, double rot,
		double hadj, const pGEcontext gc, pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int idx = get_and_increment_idx(dev);
	register_element( dev);
	double w = RAPHAEL_StrWidthUTF8(str, gc, dev);
	double h = getFontSize(gc->cex, gc->ps);
	if( h < 1.0 ) return;

 	double pi = 3.141592653589793115997963468544185161590576171875;
 	double alpha = -rot * pi / 180;

 	double Px = x + (0.5-hadj) * w;
 	double Py = y - 0.5 * h;
 	double _cos = cos( alpha );
 	double _sin = sin( alpha );

 	double Ppx = x + (Px-x) * _cos - (Py-y) * _sin ;
 	double Ppy = y + (Px-x) * _sin + (Py-y) * _cos;

 	fprintf(pd->dmlFilePointer, "var elt_%d = %s.text(", idx, pd->objectname );
 	fprintf(pd->dmlFilePointer, "%.5f,%.5f", Ppx, Ppy);
	fputs(",\"", pd->dmlFilePointer );
	raphael_textUTF8(str, pd);
	fputs("\"", pd->dmlFilePointer );
	fputs(");\n", pd->dmlFilePointer );

	RAPHAEL_SetFontSpec(dev, gc, idx);
	if( fabs( rot ) < 1 ){

	} else {
		fprintf(pd->dmlFilePointer, "elt_%d.transform(\"", idx);
		fprintf(pd->dmlFilePointer, "R-%.5f", rot);
		fputs("\");\n", pd->dmlFilePointer );
	}

	fflush(pd->dmlFilePointer);

}
static void RAPHAEL_Text(double x, double y, const char *str, double rot,
		double hadj, const pGEcontext gc, pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int idx = get_and_increment_idx(dev);
	register_element( dev);
	double w = RAPHAEL_StrWidth(str, gc, dev);
	double h = getFontSize(gc->cex, gc->ps);
	if( h < 1.0 ) return;

 	double pi = 3.141592653589793115997963468544185161590576171875;
 	double alpha = -rot * pi / 180;

 	double Px = x + (0.5-hadj) * w;
 	double Py = y - 0.5 * h;
 	double _cos = cos( alpha );
 	double _sin = sin( alpha );

 	double Ppx = x + (Px-x) * _cos - (Py-y) * _sin ;
 	double Ppy = y + (Px-x) * _sin + (Py-y) * _cos;

 	fprintf(pd->dmlFilePointer, "var elt_%d = %s.text(", idx, pd->objectname );
 	fprintf(pd->dmlFilePointer, "%.5f,%.5f", Ppx, Ppy);

	fputs(",\"", pd->dmlFilePointer );
	raphael_text(str, pd);
	fputs("\"", pd->dmlFilePointer );
	fputs(");\n", pd->dmlFilePointer );

	RAPHAEL_SetFontSpec(dev, gc, idx);
	if( fabs( rot ) < 1 ){

	} else {
		fprintf(pd->dmlFilePointer, "elt_%d.transform(\"", idx);
		fprintf(pd->dmlFilePointer, "R-%.5f", rot);
		fputs("\");\n", pd->dmlFilePointer );
	}

	fflush(pd->dmlFilePointer);

}
static void RAPHAEL_NewPage(const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	if (pd->pageNumber > 0) {
		SEXP repPackage;
		PROTECT(
				repPackage = eval(
						lang2(install("getNamespace"),
								ScalarString(mkChar("ReporteRs"))),
						R_GlobalEnv));

		SEXP RCallBack;
		PROTECT(RCallBack = allocVector(LANGSXP, 2));
		SETCAR(RCallBack, findFun(install("triggerPostCommand"), repPackage));

		SEXP env;

		PROTECT(env = coerceVector(pd->env, ENVSXP));

		SETCADR(RCallBack, env);
		SET_TAG(CDR(RCallBack), install("env"));

		PROTECT(eval(RCallBack, repPackage));

		UNPROTECT(4);
		closeFile(pd->dmlFilePointer);
	}

	int which = pd->pageNumber % pd->maxplot;
	pd->pageNumber++;
	pd->canvas_id++;
	dev->right = pd->width[which];
	dev->bottom = pd->height[which];
	dev->left = 0;
	dev->top = 0;

	dev->clipLeft = 0;
	dev->clipRight = dev->right;
	dev->clipBottom = dev->bottom;
	dev->clipTop = 0;

	pd->clippedx0 = dev->clipLeft;
	pd->clippedy0 = dev->clipTop;
	pd->clippedx1 = dev->clipRight;
	pd->clippedy1 = dev->clipBottom;

	pd->offx = pd->x[which];
	pd->offy = pd->y[which];
	pd->extx = pd->width[which];
	pd->exty = pd->height[which];

	char *filename={0};
	filename = get_raphael_filename(pd->filename, pd->pageNumber);

	pd->dmlFilePointer = (FILE *) fopen(filename, "w");
	char *canvasname={0};
	canvasname = get_raphael_canvasname(pd->canvas_id);
	if (pd->dmlFilePointer == NULL) {
		Rf_error("error while opening %s\n", filename);
	}
	updateFontInfo(dev, gc);
	pd->objectname = get_raphael_jsobject_name(pd->filename, pd->canvas_id);
	fprintf(pd->dmlFilePointer, "var %s = new Raphael(document.getElementById('%s'), %.0f, %.0f);\n"
			, pd->objectname, canvasname, dev->right, dev->bottom);

	SEXP cmdSexp = PROTECT(allocVector(STRSXP, 3));
	SET_STRING_ELT(cmdSexp, 0, mkChar(filename));
	SET_STRING_ELT(cmdSexp, 1, mkChar(pd->objectname));
	SET_STRING_ELT(cmdSexp, 2, mkChar(canvasname));

	SEXP repPackage;
	PROTECT(
			repPackage = eval(
					lang2(install("getNamespace"),
							ScalarString(mkChar("ReporteRs"))),
					R_GlobalEnv));

	SEXP RCallBack;
	PROTECT(RCallBack = allocVector(LANGSXP, 3));
	SETCAR(RCallBack, findFun(install("registerRaphaelGraph"), repPackage));

	SETCADR( RCallBack, cmdSexp );
	SET_TAG( CDR( RCallBack ), install("plot_attributes") );

	SEXP env;
	PROTECT(env = coerceVector(pd->env, ENVSXP));

	SETCADDR(RCallBack, env);
	SET_TAG( CDDR( RCallBack ), install("env") );

	PROTECT(eval(RCallBack, repPackage));

	UNPROTECT(5);


	free(filename);
	free(canvasname);

}
static void RAPHAEL_Close(pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	closeFile(pd->dmlFilePointer);
	free(pd);
}

static void RAPHAEL_Clip(double x0, double x1, double y0, double y1, pDevDesc dev) {
	dev->clipLeft = x0;
	dev->clipRight = x1;
	dev->clipBottom = y1;
	dev->clipTop = y0;
}

static void RAPHAEL_MetricInfo(int c, const pGEcontext gc, double* ascent,
		double* descent, double* width, pDevDesc dev) {
	DOC_MetricInfo(c, gc, ascent, descent, width, dev);
}

static void RAPHAEL_Size(double *left, double *right, double *bottom, double *top,
		pDevDesc dev) {
	*left = dev->left;
	*right = dev->right;
	*bottom = dev->bottom;
	*top = dev->top;
}


static double RAPHAEL_StrWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dev) {
	return DOC_StrWidthUTF8(str, gc, dev);
}
static double RAPHAEL_StrWidth(const char *str, const pGEcontext gc, pDevDesc dev) {
	return DOC_StrWidth(str, gc, dev);
}


SEXP R_RAPHAEL_Device(SEXP filename
		, SEXP width, SEXP height, SEXP offx,
		SEXP offy, SEXP pointsize, SEXP fontfamily, SEXP canvas_id, SEXP env) {

	double* w = REAL(width);
	double* h = REAL(height);

	double* x = REAL(offx);
	double* y = REAL(offy);
	int nx = length(width);

	double ps = asReal(pointsize);
	int canvasid = INTEGER(canvas_id)[0];

	BEGIN_SUSPEND_INTERRUPTS;
	GE_RAPHAELDevice(CHAR(STRING_ELT(filename, 0))
			, w, h, x, y, ps, nx, CHAR(STRING_ELT(fontfamily, 0))
			, canvasid
			, env);
	END_SUSPEND_INTERRUPTS;
	return R_NilValue;
}


