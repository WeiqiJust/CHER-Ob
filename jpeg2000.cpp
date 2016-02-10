/****************************************************************************
* RTIViewer                                                         o o     *
* Single and Multi-View Reflectance Transformation Image Viewer   o     o   *
*                                                                _   O  _   *
* Copyright	(C) 2008-2010                                          \/)\/    *
* Visual Computing Lab - ISTI CNR					              /\/|      *
* and											                     |      *
* Cultural Heritage Imaging							                 \      *
*																			*
* This program is free software: you can redistribute it and/or modify		*
* it under the terms of the GNU General Public License as published by		*
* the Free Software Foundation, either version 3 of the License, or			*
* (at your option) any later version.										*
*																			*
* This program is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of			*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				*
* GNU General Public License for more details.								*
*																			*
* You should have received a copy of the GNU General Public License			*
* along with this program.  If not, see <http://www.gnu.org/licenses/>.		*
****************************************************************************/


// local headers
#include "jpeg2000.h"
#include <stdio.h>

void Jpeg2000::setDefaults()
{
	// code stream
	codestream = NULL;
	codestream_length = 0;

	// openjpeg image
	image = NULL;

	// set default encoder/decoder values
	opj_set_default_encoder_parameters(&encparams);
	opj_set_default_decoder_parameters(&decparams);

	const char *version = opj_version();
	char *comment = new char[18+strlen(version)];
	strcpy(comment, "OpenJPEG version ");
	strcat(comment, version);
	encparams.cp_comment = comment;

	encparams.cod_format = J2K_CFMT;
	decparams.decod_format = J2K_CFMT;

	/* lossless by default */
	encparams.tcp_rates[0] = 0;
	encparams.tcp_numlayers = 1;
	encparams.cp_disto_alloc = 1;
}

void Jpeg2000::cleanCodestream()
{
	// delete stream data
	if (codestream)
		delete [] codestream;

	codestream = NULL;
	codestream_length = 0;
}

void Jpeg2000::cleanImage()
{
	// free image data
	if (image)
	{
		opj_image_destroy(image);
		image = NULL;
	}
}

Jpeg2000::Jpeg2000()
{
	setDefaults();
}

Jpeg2000::Jpeg2000(const char *filename)
{
	setDefaults();
	
	// load and decode the data stream
	if (load(filename))
		jpeg2000toImage();
}

Jpeg2000::Jpeg2000(unsigned char *bytestream, int stream_length)
{
	setDefaults();

	// store the data stream
	codestream = new unsigned char[stream_length];
	memcpy(codestream, bytestream, stream_length);
	codestream_length = stream_length;

	// decode the data stream
	jpeg2000toImage();
}

Jpeg2000::Jpeg2000(int w, int h, int prec, int bpp, int numcomps, int **components, 
									J2_COLOR_SPACE color_space, J2_TYPE jpegtype)
{
	setDefaults();

	/* create Open JPEG Image */
	/*------------------------*/

	int subsampling_dx = encparams.subsampling_dx;
	int subsampling_dy = encparams.subsampling_dy;

	int i, compno;
	opj_image_cmptparm_t *cmptparm = new opj_image_cmptparm_t[numcomps];
	
	// initialize image components
	memset(cmptparm, 0, numcomps * sizeof(opj_image_cmptparm_t));

	for(i = 0; i < numcomps; i++)
	{
		cmptparm[i].prec = prec;
		cmptparm[i].bpp = bpp;
		cmptparm[i].sgnd = 1;
		cmptparm[i].dx = subsampling_dx;
		cmptparm[i].dy = subsampling_dy;
		cmptparm[i].w = w;
		cmptparm[i].h = h;
	}

	// create the image
	image = opj_image_create(numcomps, cmptparm, (OPJ_COLOR_SPACE) color_space);

	// set image offset and reference grid
	image->x0 = encparams.image_offset_x0;
	image->y0 = encparams.image_offset_y0;
	image->x1 = encparams.image_offset_x0 + (w - 1) * subsampling_dx + 1;
	image->y1 = encparams.image_offset_y0 + (h - 1) * subsampling_dy + 1;

	// store components
	for(compno = 0; compno < numcomps; compno++)
	{
		for (i = 0; i < w * h; i++) 
			image->comps[compno].data[i] = components[compno][i];
	}

	// encode image
	encparams.cod_format = jpegtype;
	decparams.decod_format = jpegtype;
	imageToJpeg2000();
}

Jpeg2000::~Jpeg2000()
{
	if (encparams.cp_comment)
		delete [] encparams.cp_comment;

	if (encparams.cp_matrice) 
		delete [] encparams.cp_matrice;

	cleanCodestream();
	cleanImage();
}

bool Jpeg2000::load(const char *filename)
{
	bool flag = true;
	FILE *fp = fopen(filename, "rb");

	if (fp)
	{
		cleanCodestream();

		fseek(fp,0,SEEK_END);
		codestream_length = ftell(fp);
		fseek(fp,0,SEEK_SET);

		codestream = new unsigned char[codestream_length];
		fread(codestream, sizeof(unsigned char), codestream_length, fp);
	}
	else
		flag = false;

	fclose(fp);

	return flag;
}

bool Jpeg2000::save(const char *filename)
{
	bool flag = true;
	FILE *fp = fopen(filename, "wb");

	if (fp)
		fwrite(codestream, sizeof(unsigned char), codestream_length, fp);
	else
		flag = false;

	fclose(fp);

	return flag;
}

void Jpeg2000::imageToJpeg2000()
{
	/* encode the destination image */
	/* ---------------------------- */

	cleanCodestream();  // delete current codestream data (!!)

	opj_cinfo_t *cinfo;     // compressor handle
	opj_cio_t *cio = NULL;  // bytestream for writing

	// Decide if MCT should be used
	encparams.tcp_mct = image->numcomps == 3 ? 1 : 0;

	// WARNING: Cinema parameters are not handled (!!)

	if (encparams.cod_format == J2K_CFMT) 
	{
		// get a J2K compressor handle
		cinfo = opj_create_compress(CODEC_J2K);
		cinfo->event_mgr = NULL;
		cinfo->client_data = NULL;
		cinfo->jp2_handle = NULL;
		cinfo->mj2_handle = NULL;

		// setup the encoder parameters using the current image and user parameters
		opj_setup_encoder(cinfo, &encparams, image);

		// open a byte stream for writing - allocate memory for all tiles
		cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);

		// encode the image
		bool success = opj_encode(cinfo, cio, image, NULL);
		
		if (success)
		{
			codestream_length = cio_tell(cio);
			codestream = new unsigned char[codestream_length];
			memcpy(codestream, cio->buffer, codestream_length);
		}

		// close and free the byte stream
		opj_cio_close(cio);

		// free remaining compression structures
		opj_destroy_compress(cinfo);
	}
	else if (encparams.cod_format == JP2_CFMT)
	{
		// get a JP2 compressor handle
		cinfo = opj_create_compress(CODEC_JP2);
		cinfo->event_mgr = NULL;
		cinfo->client_data = NULL;

		// setup the encoder parameters using the current image and using user parameters
		opj_setup_encoder(cinfo, &encparams, image);

		// open a byte stream for writing - allocate memory for all tiles
		cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);

		// encode the image
		bool success = opj_encode(cinfo, cio, image, NULL);

		if (success) 
		{
			codestream_length = cio_tell(cio);
			codestream = new unsigned char[codestream_length];
			memcpy(codestream, cio->buffer, codestream_length);
		}

		// close and free the byte stream
		opj_cio_close(cio);

		// free remaining compression structures
		opj_destroy_compress(cinfo);
	}
}

void Jpeg2000::jpeg2000toImage()
{
	/* decode the code-stream */
	/* ---------------------- */

	cleanImage();  // delete current Open JPEG image data (!!)

	opj_dinfo_t *dinfo;     // decompressor handle
	opj_cio_t *cio = NULL;  // bytestream for reading

	if (decparams.decod_format == J2K_CFMT)
	{
		/* JPEG-2000 codestream */

		// get a decoder handle
		dinfo = opj_create_decompress(CODEC_J2K);
		dinfo->event_mgr = NULL;

		// setup the decoder decoding parameters using user parameters
		opj_setup_decoder(dinfo, &decparams);

		// open a byte stream
		cio = opj_cio_open((opj_common_ptr)dinfo, codestream, codestream_length);

		// decode the stream and fill the image structure
		image = opj_decode(dinfo, cio);

		// free decompressor 
		opj_destroy_decompress(dinfo);

		// close and free the bytestream
		opj_cio_close(cio);
	}
	else if (decparams.decod_format == JP2_CFMT)
	{
		/* JPEG 2000 compressed image data */

		// get a decoder handle
		dinfo = opj_create_decompress(CODEC_JP2);
		dinfo->event_mgr = NULL;

		// setup the decoder decoding parameters using the current image and user parameters
		opj_setup_decoder(dinfo, &decparams);

		// open a byte stream
		cio = opj_cio_open((opj_common_ptr)dinfo, codestream, codestream_length);

		// decode the stream and fill the image structure
		image = opj_decode(dinfo, cio);

		// free decompressor
		opj_destroy_decompress(dinfo);

		// close and free the byte stream
		opj_cio_close(cio);
	}
	else if (decparams.decod_format == JPT_CFMT)
	{
		/* JPEG 2000, JPIP */

		// get a decoder handle
		dinfo = opj_create_decompress(CODEC_JPT);
		dinfo->event_mgr = NULL;

		// setup the decoder decoding parameters using user parameters
		opj_setup_decoder(dinfo, &decparams);

		// open a byte stream
		cio = opj_cio_open((opj_common_ptr)dinfo, codestream, codestream_length);

		// decode the stream and fill the image structure
		image = opj_decode(dinfo, cio);

		// free decompressor
		opj_destroy_decompress(dinfo);

		// close the byte stream
		opj_cio_close(cio);
	}
}

int Jpeg2000::width()
{
	if (image && image->comps) 
		return image->comps[0].w;
	else
		return 0;
}
	

int Jpeg2000::height()
{
	if (image && image->comps)
		return image->comps[0].h;
	else
		return 0;
}

int Jpeg2000::precision()
{
	if (image && image->comps)
		return image->comps[0].prec;
	else
		return 0;
}

int Jpeg2000::numComponents()
{
	if (image)
		return image->numcomps;
	else
		return 0;
}


int* Jpeg2000::componentData(int index)
{
	if (image && index < image->numcomps)
		return image->comps[index].data;
	else
		return NULL;
}

unsigned char* Jpeg2000::streamData()
{
	return codestream;
}


int Jpeg2000::streamLength()
{
	return codestream_length;
}
