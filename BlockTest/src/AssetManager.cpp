#include "AssetManager.h"

#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"

#include "GlslInclude.h"

using namespace ly;

using namespace std;
using namespace ci;
using namespace ci::app;


bool GameAssetTexture::load()
{
	try {
		gl::Texture::Format format;
		format.enableMipmapping();
		format.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );
		mTexture = new gl::Texture( loadImage( loadAsset( mPath ) ), format );
		return true;
	} catch ( Exception& exc ) {
		console() << "Failed to load TEXTURE: " << exc.what() << std::endl;
	}
	return false;
}

bool GameAssetFont::load()
{
	try {
		mFont = new Font( loadAsset( mPath ), 30 );
		return true;
	} catch ( Exception& exc ) {
		console() << "Failed to load FONT: " << exc.what() << std::endl;
	}
	return false;
}

bool GameAssetModel::load()
{
	try {
		ObjLoader loader( loadAsset( mPath ) );
		loader.load( &mMesh );
		mVboMesh = gl::VboMesh( mMesh );
		return true;
	} catch ( Exception& exc ) {
		console() << "Failed to load MODEL: " << exc.what() << std::endl;
	}
	return false;
}


AssetManager* AssetManager::sInstance = 0;

AssetManager::AssetManager()
{
	ci::app::AppBasic* app = ci::app::AppBasic::get();
	app->addAssetDirectory( app->getAppPath() / "Contents" / "Resources" );
	app->addAssetDirectory( app->getAppPath() / ".." / "Resources" );
	
	console() << app->getAppPath().string() << std::endl;
}

AssetManager* AssetManager::get()
{
	if ( !sInstance ) sInstance = new AssetManager();
	return sInstance;
}

void AssetManager::loadAssets( std::string* assets, int numElements )
{
	for( int i = 0; i < numElements; i++ ) {
		loadAsset( assets[ i ] );
	}
}

void AssetManager::loadAssets( vector<string>& assetPaths )
{
	vector<string>::const_iterator iter;
	for( iter = assetPaths.begin(); iter != assetPaths.end(); iter++) {
		loadAsset( *iter );
	}
}

void AssetManager::loadAsset( std::string assetPath )
{
	int dotIndex = assetPath.find( "."  );
	if ( dotIndex == string::npos ) {
		console() << "Could not load asset at path '" << assetPath << "' because it does not have an extension." << std::endl;
		return;
	}
	string extension = assetPath.substr( dotIndex );
	GameAssetType type;
	if ( extension == ".jpg" || extension == ".png" ) {
		type = Texture;
	} else if ( extension == ".vert" ) {
		type = ShaderVertex;
	} else if ( extension == ".frag" ) {
		type = ShaderFragment;
	} else if ( extension == ".ttf" ) {
		type = AssetTypeFont;
	} else if ( extension == ".dae" || extension == ".obj" ) {
		type = AssetTypeModel;
	} else {
		console() << "Could not load asset at path '" << assetPath << "' because the extension is invalid." << std::endl;
		return;
	}
	
	GameAsset* asset = NULL;
	std::string path = assetPath;
	switch( type ) {
		case Texture:			asset = new GameAssetTexture();			break;
		case AssetTypeFont:		asset = new GameAssetFont();			break;
		case AssetTypeModel:	asset = new GameAssetModel();			break;
		default:break;
	}
	if ( !asset ) { return; }
	
	asset->mPath = path;
	asset->mType = type;
	if ( asset->load() ) {
		mLoadedAssets[ assetPath ] = asset;
		console() << "Asset loaded: '" << assetPath << "'" << std::endl;
	}
	else {
		delete asset;
	}
}

ci::gl::Texture* AssetManager::getTexture( const std::string path )
{
	GameAssetTexture* asset = getAsset<GameAssetTexture>( path );
	if ( asset ) {
		return asset->texture();
	}
	return NULL;
}

ci::Font* AssetManager::getFont( const std::string path )
{
	GameAssetFont* asset = getAsset<GameAssetFont>( path );
	if ( asset ) {
		return asset->font();
	}
	return NULL;
}

ci::TriMesh* AssetManager::getMesh( const std::string path )
{
	GameAssetModel* asset = getAsset<GameAssetModel>( path );
	if ( asset ) {
		return asset->mesh();
	}
	return NULL;
}

ci::gl::VboMesh* AssetManager::getVboMesh( const std::string path )
{
	GameAssetModel* asset = getAsset<GameAssetModel>( path );
	if ( asset ) {
		return asset->vboMesh();
	}
	return NULL;
}
