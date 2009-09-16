//
// C++ Implementation: PresetLoader
//
// Description:
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PresetLoader.hpp"
#include "Preset.hpp"
#include "PresetFactory.hpp"
#include <iostream>
#include <sstream>
#include <set>

#ifdef LINUX
extern "C"
{
#include <errno.h>
}
#endif

#ifdef MACOS
extern "C"
{
#include <errno.h>
}
#endif

#include <cassert>
#include "fatal.h"

#include "Common.hpp"

PresetLoader::PresetLoader (int gx, int gy, std::string dirname = std::string()) :_dirname ( dirname ), _dir ( 0 ), _ratingsSum ( 0 ), _breedabilitiesSum(0)
{
	_presetFactoryManager.initialize(gx,gy);
	// Do one scan
	if ( _dirname != std::string() )
		rescan();
}

PresetLoader::~PresetLoader()
{
	if ( _dir )
		closedir ( _dir );
}

void PresetLoader::setScanDirectory ( std::string dirname )
{
	_dirname = dirname;
}


void PresetLoader::rescan()
{
	// std::cerr << "Rescanning..." << std::endl;

	// Clear the directory entry collection
	_entries.clear();
	_presetNames.clear();
	_ratings.clear();
	_breedabilities.clear();
	_ratingsSum = 0;
	_breedabilitiesSum = 0;

	// If directory already opened, close it first
	if ( _dir )
	{
		closedir ( _dir );
		_dir = 0;
	}

	// Allocate a new a stream given the current directory name
	if ( ( _dir = opendir ( _dirname.c_str() ) ) == NULL )
	{
		handleDirectoryError();
		return; // no files loaded. _entries is empty
	}

	struct dirent * dir_entry;
	std::set<std::string> alphaSortedFileSet;
	std::set<std::string> alphaSortedPresetNameSet;

	while ( ( dir_entry = readdir ( _dir ) ) != NULL )
	{

		std::ostringstream out;
		// Convert char * to friendly string
		std::string filename ( dir_entry->d_name );

		// Verify extension is projectm or milkdrop
		if (!_presetFactoryManager.extensionHandled(parseExtension(filename)))
			continue;

		if ( filename.length() > 0 && filename[0] == '.' )
			continue;

		// Create full path name
		out << _dirname << PATH_SEPARATOR << filename;

		// Add to our directory entry collection
		alphaSortedFileSet.insert ( out.str() );
		alphaSortedPresetNameSet.insert ( filename );

		// the directory entry struct is freed elsewhere
	}

	// Push all entries in order from the file set to the file entries member (which is an indexed vector)
	for ( std::set<std::string>::iterator pos = alphaSortedFileSet.begin();
	        pos != alphaSortedFileSet.end();++pos )
		_entries.push_back ( *pos );

	// Push all preset names in similar fashion
	for ( std::set<std::string>::iterator pos = alphaSortedPresetNameSet.begin();
	        pos != alphaSortedPresetNameSet.end();++pos )
		_presetNames.push_back ( *pos );

	// Give all presets equal rating of 3 - why 3? I don't know
	_ratings = std::vector<int> ( _presetNames.size(), 3 );
	_ratingsSum = 3 * _ratings.size();

	// Give all presets equal breedability of 3 - why 3? I don't know
	_breedabilities = std::vector<int> ( _presetNames.size(), 3 );
	_breedabilitiesSum = 3 * _breedabilities.size();

	assert ( _entries.size() == _presetNames.size() );
	assert ( _ratings.size() == _entries.size() );
	assert ( _breedabilities.size() == _entries.size() );



}


std::auto_ptr<Preset> PresetLoader::loadPreset ( unsigned int index )  const
{

	// Check that index isn't insane
	assert ( index >= 0 );
	assert ( index < _entries.size() );

	// Return a new autopointer to a preset
	const std::string extension = parseExtension ( _entries[index] );

	return _presetFactoryManager.factory(extension).allocate
		( _entries[index], _presetNames[index] );

}


std::auto_ptr<Preset> PresetLoader::loadPreset ( const std::string & url )  const
{

	// Return a new autopointer to a preset
	const std::string extension = parseExtension ( url );

	/// @bug probably should not use url for preset name
	return _presetFactoryManager.factory(extension).allocate
		(url, url);

}

void PresetLoader::handleDirectoryError()
{

#ifdef WIN32
	std::cerr << "[PresetLoader] warning: errno unsupported on win32 platforms. fix me" << std::endl;
#else

	switch ( errno )
	{
		case ENOENT:
			std::cerr << "[PresetLoader] ENOENT error. The path \"" << this->_dirname << "\" probably does not exist. \"man open\" for more info." << std::endl;
			break;
		case ENOMEM:
			std::cerr << "[PresetLoader] out of memory! Are you running Windows?" << std::endl;
			abort();
		case ENOTDIR:
			std::cerr << "[PresetLoader] directory specified is not a preset directory! Trying to continue..." << std::endl;
			break;
		case ENFILE:
			std::cerr << "[PresetLoader] Your system has reached its open file limit. Trying to continue..." << std::endl;
			break;
		case EMFILE:
			std::cerr << "[PresetLoader] too many files in use by projectM! Bailing!" << std::endl;
			break;
		case EACCES:
			std::cerr << "[PresetLoader] permissions issue reading the specified preset directory." << std::endl;
			break;
		default:
			break;
	}
#endif
}

void PresetLoader::setRating ( unsigned int index, int rating )
{
	assert ( index >=0 );
	assert ( index < _ratings.size() );

	_ratingsSum -= _ratings[index];
	_ratings[index] = rating;
	_ratingsSum += rating;

	assert ( _entries.size() == _presetNames.size() );
	assert ( _ratings.size() == _entries.size() );

}

void PresetLoader::setBreedability ( unsigned int index, int breedability)
{
	assert ( index >=0 );
	assert ( index < _breedabilities.size() );

	_breedabilitiesSum -= _breedabilities[index];
	_breedabilities[index] = breedability;
	_breedabilitiesSum += breedability;

	assert ( _entries.size() == _presetNames.size() );
	assert ( _breedabilities.size() == _entries.size() );

}

unsigned int PresetLoader::addPresetURL ( const std::string & url, const std::string & presetName, int rating, int breedability )
{
	_entries.push_back ( url );
	_presetNames.push_back ( presetName );
	_ratings.push_back ( rating );
	_ratingsSum += rating;

	_breedabilities.push_back(breedability);
	_breedabilitiesSum += breedability;

	assert ( _entries.size() == _presetNames.size() );
	assert ( _ratings.size() == _entries.size() );
	assert ( _breedabilities.size() == _breedabilities.size() );

	return _entries.size()-1;
}

void PresetLoader::removePreset ( unsigned int index )
{

	_entries.erase ( _entries.begin() + index );
	_presetNames.erase ( _presetNames.begin() + index );

	_ratingsSum -= _ratings[index];
	_ratings.erase ( _ratings.begin() + index );

	_breedabilitiesSum -= _breedabilities[index];	
	_breedabilities.erase ( _breedabilities.begin() + index );

	assert ( _entries.size() == _presetNames.size() );
	assert ( _ratings.size() == _entries.size() );
	assert ( _breedabilities.size() == _entries.size() );

}

const std::string & PresetLoader::getPresetURL ( unsigned int index ) const
{
	return _entries[index];
}

const std::string & PresetLoader::getPresetName ( unsigned int index ) const
{
	return _presetNames[index];
}

int PresetLoader::getPresetRating ( unsigned int index ) const
{
	return _ratings[index];
}

const std::vector<int> & PresetLoader::getPresetBreedabilities () const
{
	return _breedabilities;
}


const std::vector<int> & PresetLoader::getPresetRatings () const
{
	return _ratings;
}


int PresetLoader::getPresetBreedabilitiesSum() const {
	return _breedabilitiesSum;
}

int PresetLoader::getPresetRatingsSum () const
{
	return _ratingsSum;
}

void PresetLoader::setPresetName(unsigned int index, std::string name) {
	_presetNames[index] = name;
}

void PresetLoader::insertPresetURL ( unsigned int index, const std::string & url, const std::string & presetName, int rating, int breedability )
{
	_entries.insert ( _entries.begin() + index, url );
	_presetNames.insert ( _presetNames.begin() + index, presetName );
	_ratings.insert ( _ratings.begin() + index, rating );
	_ratingsSum += rating;
	_breedabilities.insert ( _breedabilities.begin() + index, rating );
	_breedabilitiesSum += rating;

	assert ( _entries.size() == _presetNames.size() );
	assert ( _ratings.size() == _entries.size() );
	assert ( _breedabilities.size() == _entries.size() );
}
