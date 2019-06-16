package com.bilibili.burstlinker.sample;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import com.bilibili.burstlinker.BurstLinker;
import com.bilibili.burstlinker.GifEncodeException;
import com.bumptech.glide.Glide;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import static com.bilibili.burstlinker.BurstLinker.CPU_COUNT;

/**
 * Created by succlz123 on 2017/9/7.
 */
public class MainActivity extends AppCompatActivity {
    private static final String TAG = "JAVA_BURSTLINKER";

    private ImageView mDisplayImg;
    private TextView mTimeTv;
    private TextView mEncodeTv;
    private String mFilePath;
    private String mText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTimeTv = findViewById(R.id.time);
        mDisplayImg = findViewById(R.id.display);
        mEncodeTv = findViewById(R.id.text);

        String dstFile = "result.gif";
        mFilePath = getExternalCacheDir() + File.separator + dstFile;

        mEncodeTv.setOnClickListener(v -> {
            mEncodeTv.setEnabled(false);
            new Thread(this::encodeGIF).start();
        });
    }

    private void encodeGIF() {
        Log.i(TAG, "Start");
        long time = System.currentTimeMillis();
        final Context context = MainActivity.this;

        final Bitmap bitmap = loadBitmap(R.drawable.tcr);
        int count = 0;
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        final int delayMs = 1000;
        final BurstLinker burstLinker = new BurstLinker();

        Exception exception = null;
        try {
            burstLinker.init(width, height, mFilePath, 0, CPU_COUNT);
            burstLinker.debugLog(true);
            // select one to test
            if (true) {
                List<Bitmap> bitmaps = new ArrayList<>();
                bitmaps.add(bitmap);
                bitmaps.add(bitmap);
                bitmaps.add(bitmap);
                count = bitmaps.size();
                burstLinker.connect(bitmaps, BurstLinker.OCTREE_QUANTIZER,
                        BurstLinker.NO_DITHER, 0, 0, delayMs);
            } else {
                Bitmap colorBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(colorBitmap);
                Paint p = new Paint();
                int[] colors = new int[]{0xFFF00000, 0xFFFFFF00, 0xFFFFFFFF};
                for (int color : colors) {
                    p.setColor(color);
                    canvas.drawRect(0, 0, width, height, p);
                    count++;
                    burstLinker.connect(colorBitmap, BurstLinker.OCTREE_QUANTIZER,
                            BurstLinker.NO_DITHER, 0, 0, delayMs);
                }
            }
        } catch (GifEncodeException e) {
            e.printStackTrace();
            exception = e;
        } finally {
            burstLinker.release();
        }

        final long diff = (System.currentTimeMillis() - time);
        Log.i(TAG, "End " + diff);

        if (exception != null) {
            mText = exception.toString();
            File file = new File(mFilePath);
            if (file.exists()) {
                file.delete();
            }
        } else {
            mText =
                    "width: " + width + " height: " + height + " count: " + count + " time: " + diff + "ms";
            runOnUiThread(() -> Glide.with(context).load(mFilePath).into(mDisplayImg));
        }
        runOnUiThread(() -> {
            mTimeTv.setText(mText);
            mEncodeTv.setEnabled(true);
        });
    }

    private Bitmap loadBitmap(int resource) {
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        return BitmapFactory.decodeResource(getResources(), resource, options);
    }
}
