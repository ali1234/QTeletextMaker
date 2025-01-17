/*
 * Copyright (C) 2020-2023 Gavin MacGregor
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

#ifndef X26COMMANDS_H
#define X26COMMANDS_H

#include <QUndoCommand>

#include "document.h"
#include "x26model.h"
#include "x26triplets.h"

class InsertTripletCommand : public QUndoCommand
{
public:
	InsertTripletCommand(TeletextDocument *, X26Model *, int, int, const X26Triplet, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	X26Model *m_x26Model;
	int m_subPageIndex, m_row, m_count;
	X26Triplet m_insertedTriplet;
	bool m_firstDo;
};

class DeleteTripletCommand : public QUndoCommand
{
public:
	DeleteTripletCommand(TeletextDocument *, X26Model *, int, int, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;

private:
	TeletextDocument *m_teletextDocument;
	X26Model *m_x26Model;
	int m_subPageIndex, m_row, m_count;
	X26Triplet m_deletedTriplet;
};

class EditTripletCommand : public QUndoCommand
{
public:
	enum { Id = 201 };
	enum EditTripletEnum { ETaddress, ETmode, ETdata };

	EditTripletCommand(TeletextDocument *, X26Model *, int, int, int, int, int, QUndoCommand *parent = 0);

	void redo() override;
	void undo() override;
	bool mergeWith(const QUndoCommand *) override;
	int id() const override { return Id; }

private:
	TeletextDocument *m_teletextDocument;
	X26Model *m_x26Model;
	int m_subPageIndex, m_row, m_role;
	X26Triplet m_oldTriplet, m_newTriplet;
	bool m_firstDo;
};

#endif
