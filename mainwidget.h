/*
 * Copyright (C) 2020 Gavin MacGregor
 *
 * This file is part of QTeletextMaker.
 *
 * QTeletextMaker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QTeletextMaker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QTeletextMaker.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QBasicTimer>
#include <QFrame>
#include <QTextStream>
#include <vector>

#include "document.h"
#include "levelonepage.h"
#include "render.h"

class QPaintEvent;

class TeletextWidget : public QFrame
{
	Q_OBJECT

public:
	TeletextWidget(QFrame *parent = 0);
	~TeletextWidget();
	void setCharacter(unsigned char);
	void toggleCharacterBit(unsigned char);
	void backspaceEvent();

	QSize sizeHint() { return QSize(480+(pageRender()->leftSidePanelColumns()+pageRender()->rightSidePanelColumns())*12, 250); }

	TeletextDocument* document() const { return m_teletextDocument; }
	TeletextPageRender *pageRender() { return &m_pageRender; }

signals:
	void sizeChanged();

public slots:
	void subPageSelected();
	void refreshPage();
	void toggleReveal(bool);
	void toggleMix(bool);
	void toggleGrid(bool);
	void updateFlashTimer(int);
	void refreshRow(int);

	void setControlBit(int, bool);
	void setDefaultCharSet(int);
	void setDefaultNOS(int);
	void setDefaultScreenColour(int);
	void setDefaultRowColour(int);
	void setColourTableRemap(int);
	void setBlackBackgroundSubst(bool);
	void setSidePanelWidths(int, int);
	void setSidePanelAtL35Only(bool);
	void changeSize();

protected:
	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;
	void focusOutEvent(QFocusEvent *event) override;

	TeletextPageRender m_pageRender;

private:
	TeletextDocument* m_teletextDocument;
	TeletextPage* m_teletextPage;
	bool m_insertMode, m_grid;
	QBasicTimer m_flashTimer;
	int m_flashTiming, m_flashPhase;

	void timerEvent(QTimerEvent *event) override;

	void calculateDimensions();

	void cursorToMouse(QPoint mousePosition);
};

#endif
