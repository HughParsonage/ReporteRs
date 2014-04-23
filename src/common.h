/*
 * This file is part of ReporteRs.
 * Copyright (c) 2014, David Gohel All rights reserved.
 *
 * It is inspired from sources of R package grDevices:
 * Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
 * Copyright (C) 1998--2014  The R Core Team
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

#include <Rinternals.h>
#include <R.h>

#include <R_ext/GraphicsEngine.h>
#include "R_ext/GraphicsDevice.h"
#include <R_ext/Boolean.h>


double getFontSize(double cex, double fontsize, double lineheight);
void updateFontInfo(pDevDesc dev, R_GE_gcontext *gc);

void DOC_MetricInfo(int c, const pGEcontext gc, double* ascent,
		double* descent, double* width, pDevDesc dev);

double DOC_StrWidth(const char *str, const pGEcontext gc, pDevDesc dev);

int get_and_increment_idx(pDevDesc dev);

void register_element(pDevDesc dev);

void closeFile( FILE *file);

