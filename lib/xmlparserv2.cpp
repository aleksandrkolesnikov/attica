/*
    This file is part of KDE.

    Copyright (c) 2009 Eckhart Wörner <ewoerner@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "xmlparserv2.h"
#include <QStringList>
#include <QDebug>

#include "atticautils.h"

using namespace Attica;


template <class T>
XmlParserV2<T>::~XmlParserV2()
{
}


template <class T>
void XmlParserV2<T>::parse(const QString &data)
{
/*
        QString testxml = QString("<?xml version=\"1.0\"?>\
<ocs>\
 <meta>\
  <status>ok</status>\
  <statuscode>100</statuscode>\
  <message></message>\
 </meta>\
 <data>\
  <buildservice>\
   <id>obs</id>\
   <name>openSUSE Build Service</name>\
   <registrationurl>foobar.com</registrationurl>\
   <supportedtargets>\
    <target>openSUSE 11.2 32bit Intel</target>\
    <target>openSUSE 11.3 64bit Intel</target>\
    <target>openSUSE 11.3 32bit Intel</target>\
    <target>openSUSE 11.3 64bit Intel</target>\
   </supportedtargets>\
  </buildservice>\
  <buildservice>\
   <id>mbs</id>\
   <name>MeeGo Build Service</name>\
   <registrationurl>foobar42.com</registrationurl>\
   <supportedtargets>\
    <target>MeeGo 1.0 Intel</target>\
    <target>MeeGo 1.0 ARM</target>\
    <target>MeeGo 1.1 Intel</target>\
    <target>MeeGo 1.1 ARM</target>\
   </supportedtargets>\
  </buildservice>\
  <buildservice>\
   <id>sbs</id>\
   <name>Sebas' Build Service</name>\
   <registrationurl>foobar42.com</registrationurl>\
   <supportedtargets>\
    <target>sebasix 1.3 33bit</target>\
    <target>sebasis 4.4 14bit</target>\
    <target>sebasix 1.3 65bit</target>\
    <target>sebasis 4.4 37bit</target>\
   </supportedtargets>\
  </buildservice>\
 </data>\
</ocs>\
 ");

    qDebug() << "parsing list:" << xmlString;
    */
    QStringList elements = xmlElement();
    m_result.clear();

    QXmlStreamReader xml( data );

    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug() << "parseList():: Looking for:" << xml.name().toString();
        if (xml.isStartElement()) {
            if (xml.name() == "data") {
                while (!xml.atEnd()) {
                    xml.readNext();

                    if (xml.isEndElement() && xml.name() == "data") {
                        break;
                    }

                    if (xml.isStartElement() && elements.contains(xml.name().toString())) {
                        //qDebug() << "xxxxxxxxx New Item!" << xml.name().toString();
                        m_result.append(parseXml(xml));
                    }
                }
            } else if (xml.name() == "meta") {
                parseMetadataXml(xml);
            }
        }
    }
    if (xml.hasError()) {
        // TODO: error handling in metadata?
        qWarning() << "parseList():: XML Error: " << xml.errorString() << "\nIn XML:\n" << data;
    }
}

template <class T>
T XmlParserV2<T>::item() const
{
    if (m_result.size() == 0) {
        return T();
    }
    return m_result.at(0);
}

template <class T>
typename T::List XmlParserV2<T>::itemList() const
{
    return m_result;
}

template <class T>
void XmlParserV2<T>::parseMetadataXml(QXmlStreamReader &xml)
{
    while ( !xml.atEnd() ) {
        xml.readNext();
        if (xml.isEndElement() && xml.name() == "meta") {
            break;
        } else if (xml.isStartElement()) {
            if (xml.name() == "status") {
                m_metadata.setStatusString(xml.readElementText());
            } else if (xml.name() == "statuscode") {
                m_metadata.setStatusCode(xml.readElementText().toInt());
            } else if (xml.name() == "message") {
                m_metadata.setMessage(xml.readElementText());
            } else if (xml.name() == "totalitems") {
                m_metadata.setTotalItems(xml.readElementText().toInt());
            } else if (xml.name() == "itemsperpage") {
                m_metadata.setItemsPerPage(xml.readElementText().toInt());
            }
        }
    }
    if (xml.hasError()) {
        // TODO: error handling in metadata?
        qWarning() << "XML Error: " << xml.errorString();
    }

}

template <class T>
Metadata XmlParserV2<T>::metadata() const {
    return m_metadata;
}

template <class T>
QList<T> XmlParserV2<T>::parseXmlChildren(QXmlStreamReader& xml)
{
    QList<T> children;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xmlElement().contains(xml.name().toString())) {
                T child = parseXml(xml);
                children.append(child);
            }
        } else if (xml.isEndElement() && xml.name() == "children") {
            break;
        }
    }

    return children;
}

template <class T>
QStringList XmlParserV2<T>::xmlElement()
{
    qWarning() << "Something went wrong here. It seems like you forgot to add a specialized"
                " function for XmlParserV2::xmlElement.";
    return QStringList();
}

template <>
QStringList XmlParserV2<NoneType>::xmlElement()
{
    return QStringList();
}

template <>
QStringList XmlParserV2<AccountBalance>::xmlElement()
{
    return QStringList(QLatin1String("person"));
}

template <>
QStringList XmlParserV2<Achievement>::xmlElement()
{
    return QStringList(QLatin1String( "achievement" ));
}

template <>
QStringList XmlParserV2<Activity>::xmlElement()
{
    return QStringList(QLatin1String("activity"));
}

template <>
QStringList XmlParserV2<BuildService>::xmlElement()
{
    return QStringList(QLatin1String("buildservice")) << QLatin1String("user");
}

template <>
QStringList XmlParserV2<BuildServiceJob>::xmlElement()
{
    return QStringList(QLatin1String("buildjob")) << QLatin1String("user");
}

template <>
QStringList XmlParserV2<BuildServiceJobOutput>::xmlElement()
{
    return QStringList(QLatin1String("output"));
}

template <>
QStringList XmlParserV2<Category>::xmlElement()
{
    return QStringList(QLatin1String( "category" ));
}

template <>
QStringList XmlParserV2<Comment>::xmlElement()
{
    return QStringList(QLatin1String( "comment" ));
}

template <>
QStringList XmlParserV2<Content>::xmlElement()
{
    return QStringList(QLatin1String( "content" ));
}

template <>
QStringList XmlParserV2<Distribution>::xmlElement()
{
    return QStringList(QLatin1String( "distribution" ));
}

template <>
QStringList XmlParserV2<DownloadItem>::xmlElement()
{
    return QStringList(QLatin1String( "content" ));
}

template <>
QStringList XmlParserV2<Event>::xmlElement()
{
    return QStringList(QLatin1String( "event" ));
}

template <>
QStringList XmlParserV2<Folder>::xmlElement()
{
    return QStringList(QLatin1String( "folder" ));
}

template <>
QStringList XmlParserV2<Forum>::xmlElement()
{
    return QStringList(QLatin1String( "forum" ));
}

template <>
QStringList XmlParserV2<HomePageType>::xmlElement()
{
    return QStringList(QLatin1String( "homepagetype" ));
}

template <>
QStringList XmlParserV2<KnowledgeBaseEntry>::xmlElement()
{
    return QStringList(QLatin1String( "content" ));
}

template <>
QStringList XmlParserV2<License>::xmlElement()
{
    return QStringList(QLatin1String( "license" ));
}

template <>
QStringList XmlParserV2<Message>::xmlElement()
{
    return QStringList(QLatin1String( "message" ));
}

template <>
QStringList XmlParserV2<Person>::xmlElement()
{
    return QStringList(QLatin1String( "person" )) << QLatin1String( "user" );
}

template <>
QStringList XmlParserV2<PrivateData>::xmlElement()
{
    return QStringList(QLatin1String( "privatedata" ));
}

template <>
QStringList XmlParserV2<Project>::xmlElement()
{
    return QStringList(QLatin1String("project")) << QLatin1String("user");
}

template <>
QStringList XmlParserV2<PublisherField>::xmlElement()
{
    return QStringList(QLatin1String("field"));
}

template <>
QStringList XmlParserV2<Publisher>::xmlElement()
{
    return QStringList(QLatin1String("publisher")) << QLatin1String("user");
}

template <>
QStringList XmlParserV2<RemoteAccount>::xmlElement()
{
    return QStringList(QLatin1String("remoteaccount")) << QLatin1String("user");
}

template <>
QStringList XmlParserV2<Topic>::xmlElement()
{
    return QStringList(QLatin1String( "topic" ));
}

template <class T>
T XmlParserV2<T>::parseXml(QXmlStreamReader &xml)
{
    qWarning() << "Something went wrong here. It seems like you forgot to add a specialized"
                " function for XmlParserV2::parseXml.";
    return T();
}

template <>
AccountBalance XmlParserV2<AccountBalance>::parseXml(QXmlStreamReader& xml)
{
    AccountBalance item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == QLatin1String("balance") ) {
                item.setBalance( xml.readElementText() );
            }
            if ( xml.name() == QLatin1String("currency") ) {
                item.setCurrency( xml.readElementText() );
            }
        }
    }
    return item;
}

template <>
Achievement XmlParserV2<Achievement>::parseXml(QXmlStreamReader &xml)
{
    Achievement achievement;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                achievement.setId(xml.readElementText());
            } else if (xml.name() == "content_id") {
                achievement.setContentId(xml.readElementText());
            } else if (xml.name() == "name") {
                achievement.setName(xml.readElementText());
            } else if (xml.name() == "description") {
                achievement.setDescription(xml.readElementText());
            } else if (xml.name() == "explanation") {
                achievement.setExplanation(xml.readElementText());
            } else if (xml.name() == "points") {
                achievement.setPoints(xml.readElementText().toInt());
            } else if (xml.name() == "image") {
                achievement.setImage(QUrl(xml.readElementText()));
            } else if (xml.name() == "dependencies") {
                QStringList dependencies;
                while (!xml.atEnd()) {
                    xml.readNext();

                    if (xml.isStartElement()) {
                        if (xml.name() == "achievement_id")
                            dependencies.append(xml.readElementText());
                    } else if (xml.isEndElement() && xml.name() == "dependencies") {
                        break;
                    }
                }
                achievement.setDependencies(dependencies);
            } else if (xml.name() == "visibility") {
                achievement.setVisibility(Achievement::stringToAchievementVisibility(xml.readElementText()));
            } else if (xml.name() == "type") {
                achievement.setType(Achievement::stringToAchievementType(xml.readElementText()));
            } else if (xml.name() == "options") {
                QStringList options;
                while (!xml.atEnd()) {
                    xml.readNext();

                    if (xml.isStartElement()) {
                        if (xml.name() == "option")
                            options.append(xml.readElementText());
                    } else if (xml.isEndElement() && xml.name() == "options") {
                        break;
                    }
                }
                achievement.setOptions(options);
            } else if (xml.name() == "steps") {
                achievement.setSteps(xml.readElementText().toInt());
            } else if (xml.name() == "progress") {
                switch(achievement.type()) {
                case Achievement::FlowingAchievement:
                    achievement.setProgress(QVariant(xml.readElementText().toFloat()));
                    break;
                case Achievement::SteppedAchievement:
                    achievement.setProgress(QVariant(xml.readElementText().toInt()));
                    break;
                case Achievement::NamedstepsAchievement:
                    achievement.setProgress(QVariant(xml.readElementText()));
                    break;
                case Achievement::SetAchievement:
                    {
                    QStringList progress;
                    while (!xml.atEnd()) {
                        xml.readNext();

                        if (xml.isStartElement()) {
                            if (xml.name() == "reached")
                                progress.append(xml.readElementText());
                        } else if (xml.isEndElement() && xml.name() == "progress") {
                            break;
                        }
                    }
                    achievement.setProgress(progress);
                    }
                    break;
                default:
                    break;
                }
            }
        } else if (xml.isEndElement() && xml.name() == "achievement") {
            break;
        }
    }

    return achievement;
}

template <>
Activity XmlParserV2<Activity>::parseXml(QXmlStreamReader &xml)
{
    Activity activity;
    Person person;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                activity.setId(xml.readElementText());
            } else if (xml.name() == "personid") {
                person.setId(xml.readElementText());
            } else if (xml.name() == "avatarpic") {
                person.setAvatarUrl(xml.readElementText());
            } else if (xml.name() == "firstname") {
                person.setFirstName(xml.readElementText());
            } else if (xml.name() == "lastname") {
                person.setLastName(xml.readElementText());
            } else if (xml.name() == "timestamp") {
                QString timestampString = xml.readElementText();
                timestampString.remove(QRegExp(QLatin1String("\\+.*$")));
                QDateTime timestamp = QDateTime::fromString(timestampString, Qt::ISODate);
                activity.setTimestamp(timestamp);
            } else if (xml.name() == "message") {
                activity.setMessage(xml.readElementText());
            } else if (xml.name() == "link") {
                activity.setLink(xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "activity") {
            break;
        }
    }

    activity.setAssociatedPerson(person);
    return activity;
}

template <>
BuildService XmlParserV2<BuildService>::parseXml(QXmlStreamReader &xml)
{
    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft

    BuildService buildservice;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {

            if (xml.name() == "id") {
                buildservice.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                buildservice.setName(xml.readElementText());
            } else if (xml.name() == "registrationurl") {
                buildservice.setUrl(xml.readElementText());
            } else if (xml.name() == "supportedtargets") {
                while (!xml.atEnd()) {
                    xml.readNextStartElement();
                    if (xml.isStartElement()) {
                        if (xml.name() == "target") {
                            Target t;
                            while (!xml.atEnd()) {
                                xml.readNextStartElement();
                                if (xml.isStartElement()) {
                                    if (xml.name() == "id") {
                                        t.id = xml.readElementText();
                                    } else if (xml.name() == "name") {
                                        t.name = xml.readElementText();
                                    }
                                } else if (xml.isEndElement() && (xml.name() == "target")) {
                                    xml.readNext();
                                    break;
                                }
                            }
                            buildservice.addTarget(t);
                        }
                    } else if (xml.isEndElement() && (xml.name() == "supportedtargets")) {
                        xml.readNext();
                        break;
                    }
                }
            }
        } else if (xml.isEndElement()
               && (xml.name() == "buildservice")
               || (xml.name() == "user")) {
            break;
        }
    }
    return buildservice;
}

template <>
BuildServiceJob XmlParserV2<BuildServiceJob>::parseXml(QXmlStreamReader &xml)
{
    BuildServiceJob buildservicejob;

    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft#BuildServiceJobs
    while (!xml.atEnd()) {
        //qDebug() << "XML returned:" << xml.text().toString();
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") { // FIXME: server should give "id" here ...
                buildservicejob.setId(xml.readElementText());
            } else if (xml.name() == "project") {
                buildservicejob.setProjectId(xml.readElementText());
            } else if (xml.name() == "buildservice") {
                buildservicejob.setBuildServiceId(xml.readElementText());
            } else if (xml.name() == "target") {
                buildservicejob.setTarget(xml.readElementText());
            } else if (xml.name() == "name") {
                buildservicejob.setName(xml.readElementText());
            } else if (xml.name() == "status") {
                int status = xml.readElementText().toInt();
                buildservicejob.setStatus(status);
            } else if (xml.name() == "progress") {
                qreal progress = (qreal)(xml.readElementText().toFloat());
                buildservicejob.setProgress(progress);
            } else if (xml.name() == "message") {
                buildservicejob.setMessage(xml.readElementText());
            } else if (xml.name() == "url") {
                buildservicejob.setUrl(xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "buildjob" || xml.name() == "user")) {
            break;
        }
    }
    return buildservicejob;
}

template <>
BuildServiceJobOutput XmlParserV2<BuildServiceJobOutput>::parseXml(QXmlStreamReader &xml)
{
    BuildServiceJobOutput buildservicejoboutput;

    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft#BuildServiceJobs
    while (!xml.atEnd()) {
        if (xml.isStartElement()) {
            if (xml.name() == "output") {
                buildservicejoboutput.setOutput(xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "output") {
            break;
        }
        xml.readNext();
    }
    return buildservicejoboutput;
}

template <> Category
XmlParserV2<Category>::parseXml(QXmlStreamReader &xml)
{
    Category category;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                category.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                category.setName(xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "category") {
            break;
        }
    }

    return category;
}

template <>
Comment XmlParserV2<Comment>::parseXml(QXmlStreamReader &xml)
{
    Comment comment;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                comment.setId(xml.readElementText());
            } else if (xml.name() == "subject") {
                comment.setSubject(xml.readElementText());
            } else if (xml.name() == "text") {
                comment.setText(xml.readElementText());
            } else if (xml.name() == "childcount") {
                comment.setChildCount(xml.readElementText().toInt());
            } else if (xml.name() == "user") {
                comment.setUser(xml.readElementText());
            } else if (xml.name() == "date") {
                comment.setDate(Utils::parseQtDateTimeIso8601(xml.readElementText()));
            } else if (xml.name() == "score") {
                comment.setScore(xml.readElementText().toInt());
            } else if (xml.name() == "children") {
                QList<Comment> children = parseXmlChildren(xml);
                comment.setChildren(children);
            }
        } else if (xml.isEndElement() && xml.name() == "comment") {
            break;
        }
    }

    return comment;
}

template <>
Content XmlParserV2<Content>::parseXml(QXmlStreamReader &xml)
{
    Content content;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                content.setId( xml.readElementText());
            } else if (xml.name() == "name") {
                content.setName( xml.readElementText());
            } else if (xml.name() == "score") {
                content.setRating( xml.readElementText().toInt());
            } else if (xml.name() == "downloads") {
                content.setDownloads( xml.readElementText().toInt());
            } else if (xml.name() == "comments") {
                content.setNumberOfComments(xml.readElementText().toInt());
            } else if (xml.name() == "created") {
                // Qt doesn't accept +-Timezone modifiers, truncate if the string contains them
                QString dateString = xml.readElementText().left(19);
                content.setCreated( QDateTime::fromString( dateString, Qt::ISODate));
            } else if (xml.name() == "changed") {
                // Qt doesn't accept +-Timezone modifiers, truncate if the string contains them
                QString dateString = xml.readElementText().left(19);
                content.setUpdated( QDateTime::fromString( dateString, Qt::ISODate));
            } else if (xml.name() == "icon") {
                Icon icon;
                icon.setUrl(QUrl(xml.readElementText()));
                QXmlStreamAttributes attributes = xml.attributes();
                if (attributes.hasAttribute(QLatin1String( "width" ))) {
                    icon.setWidth(attributes.value(QLatin1String( "width" )).toString().toInt());
                }
                if (attributes.hasAttribute(QLatin1String( "height" ))) {
                    icon.setHeight(attributes.value(QLatin1String( "height" )).toString().toInt());
                }
                // append the icon to the current list of icons
                QList<Icon> icons;
                icons = content.icons();
                icons.append(icon);
                content.setIcons(icons);
            } else if (xml.name() == "video") {
                QUrl video(xml.readElementText());
                // append the video to the current list of videos
                QList<QUrl> videos;
                videos = content.videos();
                videos.append(video);
                content.setVideos(videos);
            } else {
                content.addAttribute(xml.name().toString(), xml.readElementText());
            }
        }

        if (xml.isEndElement() && xml.name() == "content") {
            break;
        }
    }

    // in case the server only sets creation date, use that as updated also
    if (content.updated().isNull()) {
        content.setUpdated(content.created());
    }

    return content;
}

template <>
Distribution XmlParserV2<Distribution>::parseXml(QXmlStreamReader &xml)
{
    Distribution item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == "id" ) {
                item.setId( xml.readElementText().toInt() );
            } else if ( xml.name() == "name" ) {
                item.setName( xml.readElementText() );
            }
        }
        if (xml.isEndElement() && xml.name() == "distribution") {
            break;
        }
    }
    return item;
}

template <>
DownloadItem XmlParserV2<DownloadItem>::parseXml(QXmlStreamReader &xml)
{
    DownloadItem item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == "downloadlink" ) {
                item.setUrl( xml.readElementText() );
            } else if ( xml.name() == "mimetype" ) {
                item.setMimeType( xml.readElementText() );
            } else if ( xml.name() == "packagename" ) {
                item.setPackageName( xml.readElementText() );
            } else if ( xml.name() == "packagerepository" ) {
                item.setPackageRepository( xml.readElementText() );
            } else if ( xml.name() == "gpgfingerprint" ) {
                item.setGpgFingerprint( xml.readElementText() );
            } else if ( xml.name() == "gpgsignature" ) {
                item.setGpgSignature( xml.readElementText() );
            } else if ( xml.name() == "downloadway" ) {
                item.setType( DownloadDescription::Type(xml.readElementText().toInt()) );
            }
        }
    }
    return item;
}

template <>
Event XmlParserV2<Event>::parseXml(QXmlStreamReader &xml)
{
    Event event;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                event.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                event.setName(xml.readElementText());
            } else if (xml.name() == "description") {
                event.setDescription(xml.readElementText());
            } else if (xml.name() == "user") {
                event.setUser(xml.readElementText());
            } else if (xml.name() == "startdate") {
                QString date = xml.readElementText().remove(QRegExp(QLatin1String( "\\+.*$" )));
                event.setStartDate(QDate::fromString(date, Qt::ISODate));
            } else if (xml.name() == "enddate") {
                QString date = xml.readElementText().remove(QRegExp(QLatin1String( "\\+.*$" )));
                event.setEndDate(QDate::fromString(date, Qt::ISODate));
            } else if (xml.name() == "latitude") {
                event.setLatitude(xml.readElementText().toFloat());
            } else if (xml.name() == "longitude") {
                event.setLongitude(xml.readElementText().toFloat());
            } else if (xml.name() == "homepage") {
                event.setHomepage(xml.readElementText());
            } else if (xml.name() == "country") {
                event.setCountry(xml.readElementText());
            } else if (xml.name() == "city") {
                event.setCity(xml.readElementText());
            } else {
                event.addExtendedAttribute(xml.name().toString(), xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "event") {
            break;
        }
    }

    return event;
}

template <>
Folder XmlParserV2<Folder>::parseXml(QXmlStreamReader &xml)
{
    Folder folder;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                folder.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                folder.setName(xml.readElementText());
            } else if (xml.name() == "messagecount") {
                folder.setMessageCount(xml.readElementText().toInt());
            } else if (xml.name() == "type") {
                folder.setType(xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "folder") {
            break;
        }
    }

    return folder;
}

template <>
Forum XmlParserV2<Forum>::parseXml(QXmlStreamReader &xml)
{
    Forum forum;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                forum.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                forum.setName(xml.readElementText());
            } else if (xml.name() == "description") {
                forum.setDescription(xml.readElementText());
            } else if (xml.name() == "date") {
                forum.setDate(Utils::parseQtDateTimeIso8601(xml.readElementText()));
            } else if (xml.name() == "icon") {
                forum.setIcon(QUrl(xml.readElementText()));
            } else if (xml.name() == "childcount") {
                forum.setChildCount(xml.readElementText().toInt());
            } else if (xml.name() == "children") {
                QList<Forum> children = parseXmlChildren(xml);
                forum.setChildren(children);
            } else if (xml.name() == "topics") {
                forum.setTopics(xml.readElementText().toInt());
            }
        } else if (xml.isEndElement() && xml.name() == "forum") {
            break;
        }
    }

    return forum;
}

template <>
HomePageType XmlParserV2<HomePageType>::parseXml(QXmlStreamReader &xml)
{
    HomePageType item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == "id" ) {
                item.setId( xml.readElementText().toInt() );
            } else if ( xml.name() == "name" ) {
                item.setName( xml.readElementText() );
            }
        }
        if (xml.isEndElement() && xml.name() == "homepagetype") {
            break;
        }
    }
    return item;
}

template <>
KnowledgeBaseEntry XmlParserV2<KnowledgeBaseEntry>::parseXml(QXmlStreamReader &xml)
{
    KnowledgeBaseEntry knowledgeBase;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                knowledgeBase.setId(xml.readElementText());
            } else if (xml.name() == "status") {
                knowledgeBase.setStatus(xml.readElementText());
            } else if (xml.name() == "contentId") {
                knowledgeBase.setContentId(xml.readElementText().toInt());
            } else if (xml.name() == "user") {
                knowledgeBase.setUser(xml.readElementText());
            } else if (xml.name() == "changed") {
                knowledgeBase.setChanged(QDateTime::fromString( xml.readElementText(), Qt::ISODate ));
            } else if (xml.name() == "description") {
                knowledgeBase.setDescription(xml.readElementText());
            } else if (xml.name() == "answer") {
                knowledgeBase.setAnswer(xml.readElementText());
            } else if (xml.name() == "comments") {
                knowledgeBase.setComments(xml.readElementText().toInt());
            } else if (xml.name() == "detailpage") {
                knowledgeBase.setDetailPage(QUrl(xml.readElementText()));
            } else if (xml.name() == "contentid") {
                knowledgeBase.setContentId(xml.readElementText().toInt());
            } else if (xml.name() == "name") {
                knowledgeBase.setName(xml.readElementText());
            } else {
                knowledgeBase.addExtendedAttribute(xml.name().toString(), xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "content") {
            break;
        }
    }

    return knowledgeBase;
}

template <>
License XmlParserV2<License>::parseXml(QXmlStreamReader &xml)
{
    License item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == "id" ) {
                item.setId( xml.readElementText().toInt() );
            } else if ( xml.name() == "name" ) {
                item.setName( xml.readElementText() );
            } else if ( xml.name() == "link" ) {
                item.setUrl( xml.readElementText() );
            }
        }
        if (xml.isEndElement() && xml.name() == "license") {
            break;
        }
    }
    return item;
}

template <>
Message XmlParserV2<Message>::parseXml(QXmlStreamReader &xml)
{
    Message message;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                message.setId(xml.readElementText());
            } else if (xml.name() == "messagefrom") {
                message.setFrom(xml.readElementText());
            } else if (xml.name() == "messageto") {
                message.setTo(xml.readElementText());
            } else if (xml.name() == "senddate") {
                message.setSent(QDateTime::fromString(xml.readElementText(), Qt::ISODate));
            } else if (xml.name() == "status") {
                message.setStatus(Message::Status(xml.readElementText().toInt()));
            } else if (xml.name() == "subject") {
                message.setSubject(xml.readElementText());
            } else if (xml.name() == "body") {
                message.setBody(xml.readElementText());
            }
        }

        if (xml.isEndElement() && xml.name() == "message") {
            break;
        }
    }

    return message;
}

template <>
Person XmlParserV2<Person>::parseXml(QXmlStreamReader &xml)
{
    Person person;
    bool hasAvatarPic = false;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "personid") {
                person.setId(xml.readElementText());
            } else if (xml.name() == "firstname") {
                person.setFirstName(xml.readElementText());
            } else if (xml.name() == "lastname") {
                person.setLastName(xml.readElementText());
            } else if (xml.name() == "homepage") {
                person.setHomepage(xml.readElementText());
            } else if (xml.name() == "avatarpic") {
                person.setAvatarUrl(xml.readElementText());
            } else if (xml.name() == "avatarpicfound") {
                QString value = xml.readElementText();
                if (value.toInt()) {
                    hasAvatarPic = true;
                }
            } else if (xml.name() == "birthday") {
                person.setBirthday(QDate::fromString(xml.readElementText(), Qt::ISODate));
            } else if (xml.name() == "city") {
                person.setCity(xml.readElementText());
            } else if (xml.name() == "country") {
                person.setCountry(xml.readElementText());
            } else if (xml.name() == "latitude") {
                person.setLatitude(xml.readElementText().toFloat());
            } else if (xml.name() == "longitude") {
                person.setLongitude(xml.readElementText().toFloat());
            } else {
                person.addExtendedAttribute(xml.name().toString(), xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "person" || xml.name() == "user")) {
            break;
        }
    }

    if (!hasAvatarPic) {
        person.setAvatarUrl(QString());
    }

    return person;
}

template <>
PrivateData XmlParserV2<PrivateData>::parseXml(QXmlStreamReader &xml)
{
    PrivateData data;
    bool hasAvatarPic = false;
    QString key;

    // TODO: when we get internet and some documentation
    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "key") {
                key = xml.readElementText();
            } else if (xml.name() == "value") {
                data.setAttribute(key, xml.readElementText());
            } else if (xml.name() == "timestamp") {
                data.setTimestamp(key, QDateTime::fromString(xml.readElementText(), Qt::ISODate));
            }
        } else if (xml.isEndElement() && (xml.name() == "data" || xml.name() == "user")) {
            break;
        }
    }

    return data;
}

template <>
Project XmlParserV2<Project>::parseXml(QXmlStreamReader &xml)
{
    Project project;

    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft#Projects
    while (!xml.atEnd()) {
        //qDebug() << "XML returned:" << xml.text().toString();
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == QLatin1String("projectid")) {
                project.setId(xml.readElementText());
            } else if (xml.name() == QLatin1String("name")) {
                project.setName(xml.readElementText());
            } else if (xml.name() == QLatin1String("version")) {
                project.setVersion(xml.readElementText());
            } else if (xml.name() == QLatin1String("license")) {
                project.setLicense(xml.readElementText());
            } else if (xml.name() == QLatin1String("url")) {
                project.setUrl(xml.readElementText());
            } else if (xml.name() == QLatin1String("summary")) {
                project.setSummary(xml.readElementText());
            } else if (xml.name() == QLatin1String("description")) {
                project.setDescription(xml.readElementText());
            } else if (xml.name() == QLatin1String("specfile")) {
                project.setSpecFile(xml.readElementText());
            } else if (xml.name() == QLatin1String("developers")) {
                project.setDevelopers(xml.readElementText().split(QLatin1String("\n")));
            } else if (xml.name() == QLatin1String("projectlist")) {
                QXmlStreamReader list_xml(xml.readElementText());
                while (!list_xml.atEnd()) {
                    list_xml.readNext();
                    if (xml.name() == QLatin1String("projectid")) {
                        project.setSpecFile(xml.readElementText());
                    }
                }

            }
        } else if (xml.isEndElement() && (xml.name() == QLatin1String("project") || xml.name() == QLatin1String("user"))) {
            break;
        }
    }
    return project;
}

template <>
PublisherField XmlParserV2<PublisherField>::parseXml(QXmlStreamReader &xml)
{
    PublisherField fld;
    while (!xml.atEnd()) {
        xml.readNextStartElement();
        if (xml.isStartElement()) {
            if (xml.name() == "fieldtype") {
                fld.setType(xml.readElementText());
            } else if (xml.name() == "name") {
                fld.setName(xml.readElementText());
            } else if (xml.name() == "data") {
                fld.setData(xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "field")) {
            xml.readNext();
            break;
        }
    }
    return fld;
}

template <>
Publisher XmlParserV2<Publisher>::parseXml(QXmlStreamReader &xml)
{
    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft

    Publisher publisher;
    QStringList fields;

    while (!xml.atEnd()) {
        //qDebug() << "XML returned:" << xml.text().toString();
        xml.readNext();

        if (xml.isStartElement()) {

            if (xml.name() == "id") {
                publisher.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                publisher.setName(xml.readElementText());
            } else if (xml.name() == "registrationurl") {
                publisher.setUrl(xml.readElementText());
            } else if (xml.name() == "fields") {
                while (!xml.atEnd()) {
                    xml.readNextStartElement();
                    if (xml.isStartElement()) {
                        if (xml.name() == "field") {
                            Field t;
                            while (!xml.atEnd()) {
                                xml.readNextStartElement();
                                if (xml.isStartElement()) {
                                    if (xml.name() == "fieldtype") {
                                        t.type = xml.readElementText();
                                    } else if (xml.name() == "name") {
                                        t.name = xml.readElementText();
                                    } else if (xml.name() == "fieldsize") {
                                        t.fieldsize = xml.readElementText().toInt();
                                    } else if (xml.name() == "required") {
                                        t.required = xml.readElementText() == QLatin1String("true");
                                    } else if (xml.name() == "options") {
                                        while (!xml.atEnd())
                                        {
                                            xml.readNextStartElement();
                                            if(xml.isStartElement())
                                            {
                                                if(xml.name() == "option")
                                                {
                                                    t.options << xml.readElementText();
                                                }
                                            } else if (xml.isEndElement() && xml.name() == "options") {
                                                xml.readNext();
                                                break;
                                            }
                                        }
                                    }
                                } else if (xml.isEndElement() && (xml.name() == "field")) {
                                    xml.readNext();
                                    break;
                                }
                            }
                            publisher.addField(t);
                        }
                    } else if (xml.isEndElement() && (xml.name() == "fields")) {
                        xml.readNext();
                        break;
                    }
                }
            } else if (xml.name() == "supportedtargets") {
                while (!xml.atEnd()) {
                    xml.readNextStartElement();
                    if (xml.isStartElement()) {
                        if (xml.name() == "target") {
                            Target t;
                            t.name = xml.readElementText();
                            publisher.addTarget(t);
                        }
                    } else if (xml.isEndElement() && (xml.name() == "supportedtargets")) {
                        xml.readNext();
                        break;
                    }
                }
            }
        } else if (xml.isEndElement()
               && (xml.name() == "publisher")
               || (xml.name() == "user")) {
            break;
        }
    }
    return publisher;
}

template <>
RemoteAccount XmlParserV2<RemoteAccount>::parseXml(QXmlStreamReader &xml)
{
    RemoteAccount remoteaccount;

    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft#RemoteAccounts
    while (!xml.atEnd()) {
        //qDebug() << "XML returned:" << xml.text().toString();
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                remoteaccount.setId(xml.readElementText());
            } else if (xml.name() == "type") {
                remoteaccount.setType(xml.readElementText());
            } else if (xml.name() == "typeid") {  // FIXME: change to remoteserviceid sometime soon (OCS API change pending
                remoteaccount.setRemoteServiceId(xml.readElementText());
            } else if (xml.name() == "data") {
                remoteaccount.setData(xml.readElementText());
            } else if (xml.name() == "login") {
                remoteaccount.setLogin(xml.readElementText());
            } else if (xml.name() == "password") {
                remoteaccount.setPassword(xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "remoteaccount" || xml.name() == "user")) {
            break;
        }
    }
    return remoteaccount;
}

template <>
Topic XmlParserV2<Topic>::parseXml(QXmlStreamReader &xml)
{
    Topic topic;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                topic.setId(xml.readElementText());
            } else if (xml.name() == "forumId") {
                topic.setForumId(xml.readElementText());
            } else if (xml.name() == "user") {
                topic.setUser(xml.readElementText());
            } else if (xml.name() == "date") {
                topic.setDate(Utils::parseQtDateTimeIso8601(xml.readElementText()));
            } else if (xml.name() == "subject") {
                topic.setSubject(xml.readElementText());
            } else if (xml.name() == "content") {
                topic.setContent(xml.readElementText());
            } else if (xml.name() == "comments") {
                topic.setComments(xml.readElementText().toInt());
            }
        } else if (xml.isEndElement() && xml.name() == "topic") {
            break;
        }
    }

    return topic;
}
