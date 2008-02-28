/***************************************************************************
 *   Copyright (C) 2007 by carm   *
 *   carmelo.piccione@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef QPROJECTM_PLAYLISTMODEL_H
#define QPROJECTM_PLAYLISTMODEL_H

#include <cassert>
#include <QAbstractTableModel>
#include <QVector>

#include <projectM.hpp>
#include <QStringList>
class QXmlStreamReader;

class QPlaylistModel : public QAbstractTableModel
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
static const int URLInfoRole = Qt::UserRole;
static const int RatingRole = Qt::UserRole+1;
     QPlaylistModel(projectM & _projectM, QObject * parent = 0);
     ~QPlaylistModel() { }
bool setData(const QModelIndex & index, const QVariant & value, int role=Qt::EditRole);

void appendRow (const QString & presetURL, const QString & presetName, int rating = 3);
void removeRow (int index);
QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

void clear();

QVariant data ( const QModelIndex & index, int role) const;
int rowCount ( const QModelIndex & parent = QModelIndex()) const ;
int columnCount ( const QModelIndex & parent= QModelIndex()) const ;

bool readPlaylist(const QString & file);
bool writePlaylist ( const QString & file );

Qt::ItemFlags flags(const QModelIndex &index) const;

inline const QString & playlistName() {
	return m_playlistName;
}


inline const QString & playlistDesc() {
	return m_playlistDesc;
}

inline void setPlaylistName(const QString & name) {
	m_playlistName = name;
}

inline void setPlaylistDesc(const QString & desc) {
	m_playlistDesc = desc;
}


inline Qt::DropActions supportedDropActions() const
{
	return Qt::MoveAction;
}

inline QStringList mimeTypes () const  {
	QStringList mimeTypes;
	mimeTypes << PRESET_MIME_TYPE;
	return mimeTypes;
}
	
	
 bool dropMimeData(const QMimeData *data, Qt::DropAction action,
				 int row, int column, const QModelIndex &parent);
void clearItems();

public slots:
	void updateItemHighlights();
	
 private:
	void readPlaylistItem(QXmlStreamReader & reader);
	static QString PRESET_MIME_TYPE;
	QVariant ratingToIcon(int rating) const;
	projectM & m_projectM;
	QVector<int> m_ratings;
	QString m_playlistName;
	QString m_playlistDesc;
};
#endif
