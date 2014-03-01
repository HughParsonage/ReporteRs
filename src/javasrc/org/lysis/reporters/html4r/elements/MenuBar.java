/*
 * This file is part of ReporteRs
 * Copyright (c) 2014, David Gohel All rights reserved.
 * This program is licensed under the GNU GENERAL PUBLIC LICENSE V3.
 * You may obtain a copy of the License at :
 * http://www.gnu.org/licenses/gpl.html
 */

package org.lysis.reporters.html4r.elements;

import java.util.Iterator;
import java.util.LinkedHashMap;

public class MenuBar implements Cloneable, HTML4R {
	private LinkedHashMap<String, String> titles;
	private String brand;
	private String activeTitle;
	
	public MenuBar(String headStr){
		titles = new LinkedHashMap<String, String>();
		brand = org.apache.commons.lang.StringEscapeUtils.escapeHtml(headStr);
	}
	
	public void addTitle(String title, String filename){
		titles.put(title, filename);
	}
	
	public void setActiveTitle(String title){
		activeTitle = title;
	}
	
	public Object clone() {
		Object o = null;
		try {
			// On r�cup�re l'instance � renvoyer par l'appel de la 
			// m�thode super.clone()
			o = super.clone();
		} catch(CloneNotSupportedException cnse) {
			// Ne devrait jamais arriver car nous impl�mentons 
			// l'interface Cloneable
			cnse.printStackTrace(System.err);
		}
		// on renvoie le clone
		return o;
	}
	/*
	<div id="navbar-div">
		<div class="navbar navbar-inverse navbar-fixed-top" role="navigation">
			<div class="container">
				<a class="navbar-brand" href="#">Brand</a>
				<button class = "navbar-toggle" data-toggle = "collapse">
					<span class = "icon-bar"></span>
					<span class = "icon-bar"></span>
					<span class = "icon-bar"></span>
				</button>
				<div class="collapse navbar-collapse">
					<ul class="nav navbar-nav navbar-left">
						<li class="active"><a href="#UIDKdU3FD75Lh">Title 1</a></li>
						<li><a href="#UIDMLGYcVtlXC">Title 2</a></li>
						<li class="dropdown"><a href="#" class="dropdown-toggle" data-toggle="dropdown">Title 3<b class="caret"></b></a>
							<ul class="dropdown-menu">
								<li><a href="#UIDaV8QZfgJmF">Title 3</a></li>
								<li><a href="#UIDaV8QZfgXXX">Title 3.1</a></li>
								
							</ul></li>
					</ul>
				</div>
			</div>
		</div>
	</div>
	*/

	public String getHTML() {
		String ul = "<ul class=\"nav navbar-nav navbar-left\">";
		for (Iterator<String> it1 = titles.keySet().iterator(); it1.hasNext();) {
			String title = it1.next();
			String link = titles.get(title);
			if( activeTitle.equals(title) )
				ul += "<li class=\"active\"><a href=\"" + link + "\">" + org.apache.commons.lang.StringEscapeUtils.escapeHtml(title) + "</a></li>";
			else ul += "<li><a href=\"" + link + "\">" + org.apache.commons.lang.StringEscapeUtils.escapeHtml(title) + "</a></li>";
		}
		ul += "</ul>";

		String temp = "<div class=\"bs-docs-nav\">" 
			+ "<div class=\"navbar navbar-inverse navbar-fixed-top\" role=\"navigation\">"
				+ "<div class=\"container\">"
					+ "<a class=\"navbar-brand\" href=\"#\">" + brand + "</a>"
					+ "<button class = \"navbar-toggle\" data-toggle = \"collapse\">"
						+ "<span class = \"icon-bar\"></span>"
						+ "<span class = \"icon-bar\"></span>"
						+ "<span class = \"icon-bar\"></span>"
					+ "</button>"
					+ "<div class=\"collapse navbar-collapse\">"
						+ ul
					+ "</div>"
				+ "</div>"
			+ "</div>"
		+ "</div>"
		+ "<div class=\"bs-footer\" role=\"contentinfo\">" 
			+ "<div class=\"container\"></div>"
		+ "</div>"
		;
		return temp;

	}

	@Override
	public String getCSS() {
		return "";
	}

	@Override
	public String getJS() {
		return "";
	}

	@Override
	public boolean hasJS() {
		return false;
	}

	@Override
	public boolean hasHTML() {
		return true;
	}

	@Override
	public boolean hasCSS() {
		return false;
	}


}
