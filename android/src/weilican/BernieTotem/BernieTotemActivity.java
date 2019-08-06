/*
  BernieTotemActivity.java
  BernieTotem Android implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/11/26 Waync created
 */

package weilican.BernieTotem;

import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;

import weilican.good.*;

public class BernieTotemActivity extends goodJniActivity
{
  static final int NATIVE_LOAD_PIC_FROM_FILE = 1;
  static final int NATIVE_LOAD_PIC_FROM_CAMERA = 2;

  static BernieTotemActivity This;

  @Override
  protected void onCreate(Bundle b)
  {
    super.onCreate(b);
    This = this;
  }

  @Override
  protected void DoChooseFile() {
    goodJniLib.create("bt.good", getAssets());
  }

  static boolean doLoadPicture(int type) {
    if (NATIVE_LOAD_PIC_FROM_FILE == type) { // Select image from gallery.
      Intent intent = new Intent();
      intent.setType("image/*");
      intent.setAction(Intent.ACTION_GET_CONTENT);
      This.startActivityForResult(Intent.createChooser(intent, "Select Picture"), NATIVE_LOAD_PIC_FROM_FILE);
    } else if (NATIVE_LOAD_PIC_FROM_CAMERA == type) { // Capture image from camera.
      Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE); 
      This.startActivityForResult(intent, NATIVE_LOAD_PIC_FROM_CAMERA); 
    }
    return false;
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    if (RESULT_OK != resultCode || null == data) {
      return;
    }
    int s[] = null;
    if (requestCode == NATIVE_LOAD_PIC_FROM_FILE && null != data.getData()) {
      Uri uri = data.getData();
      try {
        Bitmap bitmap = MediaStore.Images.Media.getBitmap(getContentResolver(), uri);
        s = goodJniActivity.imgFromBitmap(bitmap);
      } catch (Exception e) {
        e.printStackTrace();
      }
    } else if (NATIVE_LOAD_PIC_FROM_CAMERA == requestCode) {
      Bitmap photo = (Bitmap)data.getExtras().get("data"); 
      s = goodJniActivity.imgFromBitmap(photo);
    }
    if (null != s) {
      setPicSel(s);
    }
  }

  native void setPicSel(int s[]);
}
