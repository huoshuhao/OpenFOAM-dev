/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "functionEntry.H"
#include "IOstreams.H"
#include "ISstream.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineMemberFunctionSelectionTable
    (
        functionEntry,
        execute,
        dictionaryIstream
    );

    defineMemberFunctionSelectionTable
    (
        functionEntry,
        execute,
        primitiveEntryIstream
    );
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::word Foam::functionEntry::readLine(Istream& is)
{
    word s;
    dynamic_cast<ISstream&>(is).getLine(s);
    return s;
}


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

bool Foam::functionEntry::insert
(
    dictionary& parentDict,
    const string& str
)
{
    parentDict.read(IStringStream(str)());
    return true;
}


bool Foam::functionEntry::insert
(
    const dictionary& parentDict,
    primitiveEntry& thisEntry,
    const string& str
)
{
    thisEntry.read(parentDict, IStringStream(str)());
    return true;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionEntry::functionEntry
(
    const word& key,
    const dictionary& dict,
    Istream& is
)
:
    primitiveEntry
    (
        key,
        token(readLine(is), is.lineNumber())
    )
{}


// * * * * * * * * * * * * Member Function Selectors * * * * * * * * * * * * //

bool Foam::functionEntry::execute
(
    const word& functionName,
    dictionary& parentDict,
    Istream& is
)
{
    is.fatalCheck
    (
        "functionEntry::execute"
        "(const word& functionName, dictionary& parentDict, Istream&)"
    );

    if (!executedictionaryIstreamMemberFunctionTablePtr_)
    {
        cerr<< "functionEntry::execute"
            << "(const word&, dictionary&, Istream&)"
            << " not yet initialized, function = "
            << functionName.c_str() << std::endl;

        // Return true to keep reading
        return true;
    }

    executedictionaryIstreamMemberFunctionTable::iterator mfIter =
        executedictionaryIstreamMemberFunctionTablePtr_->find(functionName);

    if (mfIter == executedictionaryIstreamMemberFunctionTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown functionEntry '" << functionName
            << "' in " << is.name() << " near line " << is.lineNumber()
            << nl << nl
            << "Valid functionEntries are :" << endl
            << executedictionaryIstreamMemberFunctionTablePtr_->toc()
            << exit(FatalError);
    }

    return mfIter()(parentDict, is);
}


bool Foam::functionEntry::execute
(
    const word& functionName,
    const dictionary& parentDict,
    primitiveEntry& entry,
    Istream& is
)
{
    is.fatalCheck
    (
        "functionEntry::execute"
        "(const word&, const dictionary&, primitiveEntry&, Istream&)"
    );

    if (!executeprimitiveEntryIstreamMemberFunctionTablePtr_)
    {
        cerr<< "functionEntry::execute"
            << "(const word&, const dictionary&, primitiveEntry&, Istream&)"
            << " not yet initialized, function = "
            << functionName.c_str() << std::endl;

        // return true to keep reading anyhow
        return true;
    }

    executeprimitiveEntryIstreamMemberFunctionTable::iterator mfIter =
        executeprimitiveEntryIstreamMemberFunctionTablePtr_->find(functionName);

    if (mfIter == executeprimitiveEntryIstreamMemberFunctionTablePtr_->end())
    {
        FatalErrorInFunction
            << "Unknown functionEntry '" << functionName
            << "' in " << is.name() << " near line " << is.lineNumber()
            << nl << nl
            << "Valid functionEntries are :" << endl
            << executeprimitiveEntryIstreamMemberFunctionTablePtr_->toc()
            << exit(FatalError);
    }

    return mfIter()(parentDict, entry, is);
}


void Foam::functionEntry::write(Ostream& os) const
{
    os.indent();

    writeKeyword(os, keyword());

    for (label i=0; i<size(); ++i)
    {
        os << operator[](i);

        if (i < size()-1)
        {
            os  << token::SPACE;
        }
    }

    os  << endl;
}


// ************************************************************************* //
