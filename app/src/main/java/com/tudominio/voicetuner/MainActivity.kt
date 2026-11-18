package com.tudominio.voicetuner;

import android.content.Intent;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Start the overlay service
        startService(new Intent(this, GameTurboOverlayService.class));
        // Close activity after starting service
        finish();
    }
}
