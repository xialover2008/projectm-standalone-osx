#include "qpreseteditordialog.hpp"
#include <QFile>
#include <QMessageBox>
#include <QKeyEvent>

QPresetEditorDialog::QPresetEditorDialog(QProjectMWidget * widget, QWidget * parent, Qt::WindowFlags f): QDialog(parent,f), _qprojectMWidget(widget) {
	_ui.setupUi(this);
	
	connect(_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxHandler(QAbstractButton*)));

}

void QPresetEditorDialog::setPreset(QString url, const QModelIndex & index) {
	
	m_index = index;
	
	disconnect(_ui.presetTextEdit, 0, this, 0);
	
	m_presetUrl = url;	
	_ui.presetTextEdit->loadPresetText(url);
	
	this->setWindowTitle(QString("Preset Editor - %1 [*]").arg(url));
	this->setWindowModified(false);

	connect(_ui.presetTextEdit, SIGNAL(textChanged()), this, SLOT(updateWindowTitle()));
	connect(_ui.presetTextEdit, SIGNAL(applyRequested()), this, SLOT(saveAndNotify()), Qt::DirectConnection);
}


void QPresetEditorDialog::updateWindowTitle()  {
	this->setWindowModified(true);
}

const QString & QPresetEditorDialog::presetUrl() const {
	return m_presetUrl;
}

void QPresetEditorDialog::revertBuffer() {
	setPreset(m_presetUrl, m_index);
}

void QPresetEditorDialog::saveFile() {
	QFile qfile(presetUrl());
	
	if (!qfile.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QMessageBox::warning (0, "Preset File Error", QString(tr("There was a problem trying to save the preset \"%1\".  The file may no longer exist or you may not have permission to read the file.")).arg(presetUrl()));
		return ;
	}
	
	QTextStream textStream(&qfile);
	
	textStream << _ui.presetTextEdit->toPlainText();
	
	textStream.flush();
	this->setWindowModified(false);
}

void QPresetEditorDialog::saveAndNotify() {

	qDebug() << "save and notify";
	saveFile();
	
	emit(presetModified(m_index));
}

void QPresetEditorDialog::buttonBoxHandler(QAbstractButton * button) {
	switch (_ui.buttonBox->standardButton(button)) {
		case QDialogButtonBox::Close:
			this->hide();
			break;
		case QDialogButtonBox::Apply:
			saveAndNotify();
			break;
		case QDialogButtonBox::Reset:
			revertBuffer();
			break;
		default:
			break;
	}
}

