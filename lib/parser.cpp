/*
    This file is part of KDE.

    Copyright (c) 2009 Eckhart Wörner <ewoerner@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
    USA.
*/

#include "parser.h"


using namespace Attica;


template <class T>
Parser<T>::~Parser()
{
}


template <class T>
T Parser<T>::parse(const QString& xmlString)
{
    QString element = xmlElement();
    T item;

    QXmlStreamReader xml(xmlString);

    while (!xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement() && xml.name() == element) {
            item = parseXml(xml);
        }
    }

    return item;
}


template <class T>
typename T::List Parser<T>::parseList(const QString& xmlString)
{
    QString element = xmlElement();
    typename T::List items;

    QXmlStreamReader xml( xmlString );

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "data") {
            while (!xml.atEnd()) {
                xml.readNext();

                if (xml.isEndElement() && xml.name() == "data") {
                    break;
                }

                if (xml.isStartElement() && xml.name() == element) {
                    items.append(parseXml(xml));
                }
            }
        }
    }

    return items;
}
