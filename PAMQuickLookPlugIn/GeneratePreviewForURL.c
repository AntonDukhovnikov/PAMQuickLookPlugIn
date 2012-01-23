//
//  GeneratePreviewForURL.c
//  PAMQuickLookPlugIn
//
//  Created by Anton Dukhovnikov on 21/01/12.
//  Copyright (c) 2012 Anton Dukhovnikov All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "PAMReader.h"

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
    CGImageRef image = CreateImageWithURL(url);
    if (!image) return noErr;
    
    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    CGSize size = CGSizeMake(width, height);
    
    CGContextRef cgContext = QLPreviewRequestCreateContext(preview, size, true, options);
    if(cgContext) 
    {
        CGContextDrawImage(cgContext, CGRectMake(0, 0, width, height), image);
        QLPreviewRequestFlushContext(preview, cgContext);        
        CFRelease(cgContext);
    }
    
    CGImageRelease(image);
    return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
    // Implement only if supported
}
