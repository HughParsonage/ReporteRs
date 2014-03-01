/*
 * This file is part of ReporteRs
 * Copyright (c) 2014, David Gohel All rights reserved.
 * This program is licensed under the GNU GENERAL PUBLIC LICENSE V3.
 * You may obtain a copy of the License at :
 * http://www.gnu.org/licenses/gpl.html
 */

package org.lysis.reporters.pptx4r.elements;

import org.pptx4j.jaxb.Context;

public class SlideNumber {

	private static String title = 
			"<p:sp xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\">"
			+ "<p:nvSpPr>"
			+ "<p:cNvPr id=\"${element_id}\" name=\"Slide Number ${element_id}\" />"
			+ "<p:cNvSpPr/>"
			+ "<p:nvPr>"
				+ "<p:ph type=\"sldNum\" sz=\"half\" idx=\"${idx}\"/>"
			+ "</p:nvPr>"
		+ "</p:nvSpPr>"
		+ "<p:spPr />"
		+ "<p:txBody>"
			+ "<a:bodyPr />"
			+ "<a:lstStyle />"
			+ "<a:p>"
				+ "<a:r>"
					+ "<a:rPr />"
					+ "<a:t>${text}</a:t>"
				+ "</a:r>"
			+ "</a:p>"
		+ "</p:txBody>"
	+ "</p:sp>";

	/**
	 * @param args
	 */
	public static Object getShape(long idx, long shape_id, String text) throws Exception{
		String value =  org.apache.commons.lang.StringEscapeUtils.escapeHtml(text);

		java.util.HashMap<String, String>mappings = new java.util.HashMap<String, String>();
        mappings.put("element_id", shape_id+""  );//4
        mappings.put("idx", idx+""  );//4
        mappings.put("text", value );
        
        Object o = org.docx4j.XmlUtils.unmarshallFromTemplate(title, mappings, Context.jcPML) ;

        return o;
	}
}
