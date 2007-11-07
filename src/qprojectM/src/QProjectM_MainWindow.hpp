/***************************************************************************
 *   Copyright (C) 2007 by carm   *
 *   carm@localhost   *
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


#ifndef QPROJECTM_MAINWINDOW_H
#define QPROJECTM_MAINWINDOW_H
#define CONFIG_FILE "/share/projectM/config.inp"

#include "ui_QProjectM_MainWindow.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <cassert>
#include "libprojectM/projectM.hpp"

#include <QGLWidget>

class QAction;
class QMenu;
class QPresetFileDialog;
class QPlaylistFileDialog;
class QPlaylistModel;

#include <iostream>


class QProjectM : public QObject, public projectM {
	Q_OBJECT

	public:
		 QProjectM(const std::string & config_file):projectM(config_file) {} 
        
		 
		 void presetSwitchedEvent(bool hardCut, unsigned int index) const {
			presetSwitchedSignal(hardCut, index);
		 }
	signals:
		void presetSwitchedSignal(bool hardCut, unsigned int index) const;
	public slots:
			
};

class QProjectMWidget : public QGLWidget
 {
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     QProjectMWidget(const std::string & _config_file, QWidget *parent)
         : QGLWidget(parent), config_file(_config_file), m_projectM(0) {}
     ~QProjectMWidget() { if (m_projectM) delete(m_projectM); }

     QProjectM * getQProjectM() { return m_projectM; }

 public slots:
     
     void setPresetLock(int state) {
		m_projectM->setPresetLock((bool)state);
		presetLockChanged((bool)state);
     }
  signals:
	void projectM_Initialized();
	void presetLockChanged(bool isLocked);
 private:
	std::string config_file;
	QProjectM * m_projectM;

 protected:



void keyReleaseEvent ( QKeyEvent * e )  {
	
	projectMKeycode pkey;
    bool ignore = false;
    switch (e->key()) {
	case Qt::Key_F4:
		pkey =  PROJECTM_K_F4;
		break;
	case Qt::Key_F3:
		pkey =  PROJECTM_K_F3;
		break;
	case Qt::Key_F2:
		pkey =  PROJECTM_K_F2;
		break;
	case Qt::Key_F1:
		pkey =  PROJECTM_K_F1;
		break;
	case Qt::Key_R:
		pkey =  PROJECTM_K_r;
		break;
	case Qt::Key_L:
		pkey =  PROJECTM_K_l;
		ignore = true;
		break;	
	case Qt::Key_N:
  		pkey =  PROJECTM_K_n;
		break;
	case Qt::Key_P:	
		pkey =  PROJECTM_K_p;
		break;		
	default:
		e->ignore();
		return;
    }
       projectMModifier modifier;

	//std::cerr << "HERE with key " << e->key() << std::endl;
	m_projectM->key_handler(PROJECTM_KEYDOWN, pkey, modifier);
	if (ignore)
		e->ignore();

	
     }

     void initializeGL()
     {
	this->m_projectM = new QProjectM(config_file);
	projectM_Initialized();
     }

     void resizeGL(int w, int h)
     {
         // setup viewport, projection etc.:
	/// @bug is setup necessary on resize or should just be in initializeGL() ?
	setup_opengl(w,h);
	m_projectM->projectM_resetGL(  w, h ); 
     }

     void paintGL()
     {
   	m_projectM->renderFrame();
     }

   private:

 void setup_opengl( int w, int h )
{
   
    /* Our shading model--Gouraud (smooth). */
     glShadeModel( GL_SMOOTH);
    /* Culling. */
    //    glCullFace( GL_BACK );
    //    glFrontFace( GL_CCW );
    //    glEnable( GL_CULL_FACE );
    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );
    /* Setup our viewport. */
     glViewport( 0, 0, w, h );
    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    //    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  
   
    //    glFrustum(0.0, height, 0.0,width,10,40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

glDrawBuffer(GL_BACK); 
  glReadBuffer(GL_BACK); 
  glEnable(GL_BLEND); 

     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
     // glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//   glClear(GL_COLOR_BUFFER_BIT);
 
  // glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,texsize,texsize,0);
  //glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,texsize,texsize);
   glLineStipple(2, 0xAAAA);
  
    
}


 };

class QProjectM_MainWindow:public QMainWindow
{
      Q_OBJECT

public:
      
      QProjectM_MainWindow(const std::string & config_file);
      ~QProjectM_MainWindow();

      void keyReleaseEvent ( QKeyEvent * e );
      QProjectM * getQProjectM();

    void refreshPlaylist();

    
protected:

      typedef struct PlaylistItemMetaData {
		PlaylistItemMetaData() {}
		PlaylistItemMetaData(const QString & _url, const QString & _name, int _rating):
			url(_url), name(_name), rating(_rating) {}

		QString url;
		QString name;
		int rating;
      } PlaylistItemMetaData;

      typedef QVector<PlaylistItemMetaData> PlaylistItemVector;
      void closeEvent(QCloseEvent *event);

private slots:
      void clearPlaylist();
      void addPresets();
      void openPlaylist();
      void savePlaylist(const QString & file = QString());
      void about();
      void postProjectM_Initialize();
      void updatePlaylistSelection(bool hardCut, int index);
      void selectPlaylistItem(const QModelIndex & index);
      void changeRating(const QModelIndex & index);

      void updateFilteredPlaylist(const QString & text);
     
private:
	void copyPlaylist();
	
      QString m_currentPlaylistFile;

      QPlaylistModel * playlistModel;
      Ui::qProjectM_MainWindow ui;

      QHash<QString, PlaylistItemVector*> historyHash;

      int oldPresetIndex;
      QTimer * m_timer;
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      void loadFile(const QString &fileName, int rating = 3);
      QString strippedName(const QString &fullFileName);

      QProjectMWidget * m_QProjectMWidget;

      QMenu *fileMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;

      QAction *openAct;;
      QAction *exitAct;
      QAction *aboutAct;
      QAction *aboutQtAct;

      QString previousFilter;

      QPresetFileDialog * m_QPresetFileDialog;
      QPlaylistFileDialog * m_QPlaylistFileDialog;
};

#endif
