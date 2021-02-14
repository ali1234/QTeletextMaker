/*
 * Copyright (C) 2020, 2021 Gavin MacGregor
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

#include "levelonecommands.h"

#include "document.h"

TypeCharacterCommand::TypeCharacterCommand(TeletextDocument *teletextDocument, unsigned char newCharacter, bool insertMode, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();
	m_columnStart = m_columnEnd = teletextDocument->cursorColumn();
	m_newCharacter = newCharacter;
	m_insertMode = insertMode;

	for (int c=0; c<40; c++)
		m_oldRowContents[c] = m_newRowContents[c] = m_teletextDocument->currentSubPage()->character(m_row, c);

	if (m_insertMode)
		setText(QObject::tr("insert character"));
	else
		setText(QObject::tr("overwrite character"));
	m_firstDo = true;
}

void TypeCharacterCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	// Only apply the typed character on the first do, m_newRowContents will remember it if we redo
	if (m_firstDo) {
		if (m_insertMode) {
			// Insert - Move characters rightwards
			for (int c=39; c>m_columnEnd; c--)
				m_newRowContents[c] = m_newRowContents[c-1];
		}
		m_newRowContents[m_columnEnd] = m_newCharacter;
		m_firstDo = false;
	}
	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_newRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_columnEnd);
	m_teletextDocument->cursorRight();
	emit m_teletextDocument->contentsChange(m_row);
}

void TypeCharacterCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_oldRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_columnStart);
	emit m_teletextDocument->contentsChange(m_row);
}

bool TypeCharacterCommand::mergeWith(const QUndoCommand *command)
{
	const TypeCharacterCommand *newerCommand = static_cast<const TypeCharacterCommand *>(command);

	// Has to be the next typed column on the same row
	if (m_subPageIndex != newerCommand->m_subPageIndex || m_row != newerCommand->m_row || m_columnEnd != newerCommand->m_columnEnd-1)
		return false;

	m_columnEnd = newerCommand->m_columnEnd;
	for (int c=0; c<40; c++)
		m_newRowContents[c] = newerCommand->m_newRowContents[c];

	return true;
}


ToggleMosaicBitCommand::ToggleMosaicBitCommand(TeletextDocument *teletextDocument, unsigned char bitToToggle, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();
	m_column = teletextDocument->cursorColumn();
	m_oldCharacter = teletextDocument->currentSubPage()->character(m_row, m_column);
	if (bitToToggle == 0x20 || bitToToggle == 0x7f)
		m_newCharacter = bitToToggle;
	else
		m_newCharacter = m_oldCharacter ^ bitToToggle;

	setText(QObject::tr("mosaic"));

}

void ToggleMosaicBitCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->currentSubPage()->setCharacter(m_row, m_column, m_newCharacter);

	m_teletextDocument->moveCursor(m_row, m_column);
	emit m_teletextDocument->contentsChange(m_row);
}

void ToggleMosaicBitCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->currentSubPage()->setCharacter(m_row, m_column, m_oldCharacter);

	m_teletextDocument->moveCursor(m_row, m_column);
	emit m_teletextDocument->contentsChange(m_row);
}

bool ToggleMosaicBitCommand::mergeWith(const QUndoCommand *command)
{
	const ToggleMosaicBitCommand *newerCommand = static_cast<const ToggleMosaicBitCommand *>(command);

	if (m_subPageIndex != newerCommand->m_subPageIndex || m_row != newerCommand->m_row || m_column != newerCommand->m_column)
		return false;
	m_newCharacter = newerCommand->m_newCharacter;
	return true;
}


BackspaceKeyCommand::BackspaceKeyCommand(TeletextDocument *teletextDocument, bool insertMode, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();
	m_columnStart = teletextDocument->cursorColumn()-1;
	if (m_columnStart == -1) {
		m_columnStart = 39;
		if (--m_row == 0)
			m_row = 24;
	}
	m_columnEnd = m_columnStart;
	m_insertMode = insertMode;

	for (int c=0; c<40; c++)
		m_oldRowContents[c] = m_newRowContents[c] = m_teletextDocument->currentSubPage()->character(m_row, c);

	setText(QObject::tr("backspace"));
	m_firstDo = true;
}

void BackspaceKeyCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	if (m_firstDo) {
		if (m_insertMode) {
			// Insert - Move characters leftwards and put a space on the far right
			for (int c=m_columnEnd; c<39; c++)
				m_newRowContents[c] = m_newRowContents[c+1];
			m_newRowContents[39] = 0x20;
		} else
			// Replace - Overwrite backspaced character with a space
			m_newRowContents[m_columnEnd] = 0x20;
		m_firstDo = false;
	}

	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_newRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_columnEnd);
	emit m_teletextDocument->contentsChange(m_row);
}

void BackspaceKeyCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_oldRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_columnStart);
	m_teletextDocument->cursorRight();
	emit m_teletextDocument->contentsChange(m_row);
}

bool BackspaceKeyCommand::mergeWith(const QUndoCommand *command)
{
	const BackspaceKeyCommand *newerCommand = static_cast<const BackspaceKeyCommand *>(command);

	// Has to be the next backspaced column on the same row
	if (m_subPageIndex != newerCommand->m_subPageIndex || m_row != newerCommand->m_row || m_columnEnd != newerCommand->m_columnEnd+1)
		return false;

	// For backspacing m_columnStart is where we began backspacing and m_columnEnd is where we ended backspacing
	// so m_columnEnd will be less than m_columnStart
	m_columnEnd = newerCommand->m_columnEnd;
	for (int c=0; c<40; c++)
		m_newRowContents[c] = newerCommand->m_newRowContents[c];

	return true;
}


DeleteKeyCommand::DeleteKeyCommand(TeletextDocument *teletextDocument, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();
	m_column = teletextDocument->cursorColumn();

	for (int c=0; c<40; c++)
		m_oldRowContents[c] = m_newRowContents[c] = m_teletextDocument->currentSubPage()->character(m_row, c);

	setText(QObject::tr("delete"));
}

void DeleteKeyCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	// Move characters leftwards and put a space on the far right
	for (int c=m_column; c<39; c++)
		m_newRowContents[c] = m_newRowContents[c+1];
	m_newRowContents[39] = 0x20;

	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_newRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_column);
	emit m_teletextDocument->contentsChange(m_row);
}

void DeleteKeyCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);

	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_oldRowContents[c]);

	m_teletextDocument->moveCursor(m_row, m_column);
	emit m_teletextDocument->contentsChange(m_row);
}

bool DeleteKeyCommand::mergeWith(const QUndoCommand *command)
{
	const DeleteKeyCommand *newerCommand = static_cast<const DeleteKeyCommand *>(command);

	if (m_subPageIndex != newerCommand->m_subPageIndex || m_row != newerCommand->m_row || m_column != newerCommand->m_column)
		return false;

	for (int c=0; c<40; c++)
		m_newRowContents[c] = newerCommand->m_newRowContents[c];

	return true;
}


InsertRowCommand::InsertRowCommand(TeletextDocument *teletextDocument, bool copyRow, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();
	m_copyRow = copyRow;

	if (m_copyRow)
		setText(QObject::tr("insert copy row"));
	else
		setText(QObject::tr("insert blank row"));
}

void InsertRowCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->moveCursor(m_row, -1);
	// Store copy of the bottom row we're about to push out, for undo
	for (int c=0; c<40; c++)
		m_deletedBottomRow[c] = m_teletextDocument->currentSubPage()->character(23, c);
	// Move lines below the inserting row downwards without affecting the FastText row
	for (int r=22; r>=m_row; r--)
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(r+1, c, m_teletextDocument->currentSubPage()->character(r, c));
	if (!m_copyRow)
		// The above movement leaves a duplicate of the current row, so blank it if requested
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(m_row, c, ' ');

	emit m_teletextDocument->refreshNeeded();
}

void InsertRowCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->moveCursor(m_row, -1);
	// Move lines below the deleting row upwards without affecting the FastText row
	for (int r=m_row; r<23; r++)
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(r, c, m_teletextDocument->currentSubPage()->character(r+1, c));
	// Now repair the bottom row we pushed out
	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(23, c, m_deletedBottomRow[c]);

	emit m_teletextDocument->refreshNeeded();
}


DeleteRowCommand::DeleteRowCommand(TeletextDocument *teletextDocument, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_row = teletextDocument->cursorRow();

	setText(QObject::tr("delete row"));
}

void DeleteRowCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->moveCursor(m_row, -1);
	// Store copy of the row we're going to delete, for undo
	for (int c=0; c<40; c++)
		m_deletedRow[c] = m_teletextDocument->currentSubPage()->character(m_row, c);
	// Move lines below the deleting row upwards without affecting the FastText row
	for (int r=m_row; r<23; r++)
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(r, c, m_teletextDocument->currentSubPage()->character(r+1, c));
	// If we deleted the FastText row blank that row, otherwise blank the last row
	int blankingRow = (m_row < 24) ? 23 : 24;
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(blankingRow, c, ' ');

	emit m_teletextDocument->refreshNeeded();
}

void DeleteRowCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->moveCursor(m_row, -1);
	// Move lines below the inserting row downwards without affecting the FastText row
	for (int r=22; r>=m_row; r--)
		for (int c=0; c<40; c++)
			m_teletextDocument->currentSubPage()->setCharacter(r+1, c, m_teletextDocument->currentSubPage()->character(r, c));
	// Now repair the row we deleted
	for (int c=0; c<40; c++)
		m_teletextDocument->currentSubPage()->setCharacter(m_row, c, m_deletedRow[c]);

	emit m_teletextDocument->refreshNeeded();
}


InsertSubPageCommand::InsertSubPageCommand(TeletextDocument *teletextDocument, bool afterCurrentSubPage, bool copySubPage, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_newSubPageIndex = teletextDocument->currentSubPageIndex()+afterCurrentSubPage;
	m_copySubPage = copySubPage;

	setText(QObject::tr("insert subpage"));

}

void InsertSubPageCommand::redo()
{
	m_teletextDocument->insertSubPage(m_newSubPageIndex, m_copySubPage);

	m_teletextDocument->selectSubPageIndex(m_newSubPageIndex, true);
}

void InsertSubPageCommand::undo()
{
	m_teletextDocument->deleteSubPage(m_newSubPageIndex);
	//TODO should we always wrench to "subpage viewed when we inserted"? Or just if subpage viewed is being deleted?

	m_teletextDocument->selectSubPageIndex(qMin(m_newSubPageIndex, m_teletextDocument->numberOfSubPages()-1), true);
}


DeleteSubPageCommand::DeleteSubPageCommand(TeletextDocument *teletextDocument, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageToDelete = teletextDocument->currentSubPageIndex();
	setText(QObject::tr("delete subpage"));
}

void DeleteSubPageCommand::redo()
{
	m_teletextDocument->deleteSubPageToRecycle(m_subPageToDelete);
	m_teletextDocument->selectSubPageIndex(qMin(m_subPageToDelete, m_teletextDocument->numberOfSubPages()-1), true);
}

void DeleteSubPageCommand::undo()
{
	m_teletextDocument->unDeleteSubPageFromRecycle(m_subPageToDelete);
	m_teletextDocument->selectSubPageIndex(m_subPageToDelete, true);
}


SetColourCommand::SetColourCommand(TeletextDocument *teletextDocument, int colourIndex, int newColour, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_colourIndex = colourIndex;
	m_oldColour = teletextDocument->currentSubPage()->CLUT(colourIndex);
	m_newColour = newColour;

	setText(QObject::tr("colour change"));

}

void SetColourCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->currentSubPage()->setCLUT(m_colourIndex, m_newColour);

	emit m_teletextDocument->colourChanged(m_colourIndex);
	emit m_teletextDocument->refreshNeeded();
}

void SetColourCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	m_teletextDocument->currentSubPage()->setCLUT(m_colourIndex, m_oldColour);

	emit m_teletextDocument->colourChanged(m_colourIndex);
	emit m_teletextDocument->refreshNeeded();
}


ResetCLUTCommand::ResetCLUTCommand(TeletextDocument *teletextDocument, int colourTable, QUndoCommand *parent) : QUndoCommand(parent)
{
	m_teletextDocument = teletextDocument;
	m_subPageIndex = teletextDocument->currentSubPageIndex();
	m_colourTable = colourTable;
	for (int i=m_colourTable*8; i<m_colourTable*8+8; i++)
		m_oldColourEntry[i&7] = teletextDocument->currentSubPage()->CLUT(i);

	setText(QObject::tr("CLUT %1 reset").arg(m_colourTable));
}

void ResetCLUTCommand::redo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	for (int i=m_colourTable*8; i<m_colourTable*8+8; i++) {
		m_teletextDocument->currentSubPage()->setCLUT(i, m_teletextDocument->currentSubPage()->CLUT(i, 0));
		emit m_teletextDocument->colourChanged(i);
	}

	emit m_teletextDocument->refreshNeeded();
}

void ResetCLUTCommand::undo()
{
	m_teletextDocument->selectSubPageIndex(m_subPageIndex);
	for (int i=m_colourTable*8; i<m_colourTable*8+8; i++) {
		m_teletextDocument->currentSubPage()->setCLUT(i, m_oldColourEntry[i&7]);
		emit m_teletextDocument->colourChanged(i);
	}

	emit m_teletextDocument->refreshNeeded();
}
