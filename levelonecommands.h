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

#ifndef LEVELONECOMMANDS_H
#define LEVELONECOMMANDS_H

#include <QUndoCommand>

#include "document.h"

class OverwriteCharacterCommand : public QUndoCommand
{
public:
	OverwriteCharacterCommand(TeletextDocument *, unsigned char, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	unsigned char m_oldCharacter, m_newCharacter;
	int m_subPageIndex, m_row, m_column;
};

class ToggleMosaicBitCommand : public QUndoCommand
{
public:
	enum { Id = 2 };

	ToggleMosaicBitCommand(TeletextDocument *, unsigned char, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;
	bool mergeWith(const QUndoCommand *) override;
	int id() const override { return Id; }

private:
	TeletextDocument *m_teletextDocument;
	unsigned char m_oldCharacter, m_newCharacter;
	int m_subPageIndex, m_row, m_column;
};

class BackspaceCommand : public QUndoCommand
{
public:
	BackspaceCommand(TeletextDocument *, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	unsigned char m_deletedCharacter;
	int m_subPageIndex, m_row, m_column;
};

class InsertSubPageCommand : public QUndoCommand
{
public:
	InsertSubPageCommand(TeletextDocument *, bool, bool, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	int m_newSubPageIndex;
	bool m_copySubPage;
};

class InsertRowCommand : public QUndoCommand
{
public:
	InsertRowCommand(TeletextDocument *, bool, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	int m_subPageIndex, m_row;
	bool m_copyRow;
	unsigned char m_deletedBottomRow[40];
};

class DeleteRowCommand : public QUndoCommand
{
public:
	DeleteRowCommand(TeletextDocument *, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	int m_subPageIndex, m_row;
	unsigned char m_deletedRow[40];
};

class SetColourCommand : public QUndoCommand
{
public:
	SetColourCommand(TeletextDocument *, int, int, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	int m_subPageIndex, m_colourIndex, m_oldColour, m_newColour;
};

class ResetCLUTCommand : public QUndoCommand
{
public:
	ResetCLUTCommand(TeletextDocument *, int, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	int m_subPageIndex, m_colourTable;
	int m_oldColourEntry[8];
};

#endif