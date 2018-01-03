# 使用方法

首先请看 [ndk 使用指南](https://developer-forum.rokid.com/t/topic/97)

# 自定义激活词

自动编译后会生成Android 标准应用程序: system/priv-app/openvoice_process/openvoice_process.apk

同时如下⽬录下拿到 BearKid Jar, 此 Jar 包获取 NLP 消息，自定义激活词接口等
```
out/target/common/obj/JAVA_LIBRARIES/BearKidAdapter_intermediates/classes.jar
```

如在 905d 开发板上，要开始使用自定义激活词，首先得先在您的设备上加载openvoice_process.apk，命令如下
```
adb root
adb remount
adb shell mkdir /system/priv-app/openvoice_process/
adb push openvoice_process.apk /system/priv-app/openvoice_process/
reboot
```
## 接口使用注意事项

1. 在您的 libs 添加 jar 包 BearKid Jar
2. 需要 import 以下三个类

```
import com.rokid.voicerec.BearKidAdapter;
import com.rokid.voicerec.BearKidCallback;
import com.rokid.voicerec.CustomWord;
```

## API

```c
int addCustomWord(int type, String word, String pinyin);
// 添加自定义激活词接口
// 参数type: 有3种 TYPE_VT_WORD (唤醒激活词), TYPE_EXIT_WORD (推出激活词), TYPE_HOT_WORD (热词激活词）
// 参数word: 激活词中文文字   如:小月亮
// 参数pinyin: 激活词拼音+声调 如:xiao3yue4liang4

int removeCustomWord(String word);
// 删除自定义激活词接口,参数 word: 激活词中文文字

List<CustomWord> queryCustomWord(int type);
// 查询自定义激活词接口，参数type：同上

## 示例

```c
package com.example.demo;

import android.app.Activity;
import android.os.Bundle;

import com.rokid.voicerec.BearKidAdapter;
import com.rokid.voicerec.BearKidCallback;
import com.rokid.voicerec.CustomWord;

public class MainActivity extends Activity implements BearKidCallback {

    public static final String TAG = "BEAR_KID_ACTIVITY";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        new Thread(new Runnable() {
            @Override
            public void run() {
                BearKidAdapter bearKidAdapter = new BearKidAdapter();
                boolean initialize = bearKidAdapter.initialize(getApplicationContext(), "com.rokid.openvoice.VoiceService", MainActivity.this);
                int i = bearKidAdapter.addCustomWord(CustomWord.TYPE_VT_WORD, "小月亮", "xiao3yue4liang4");
            }
        }).start();
    }

    @Override
    public void onVoiceEvent(int i, int i1, double v) {

    }

    @Override
    public void onIntermediateResult(String s) {

    }

    @Override
    public void onRecognizeResult(String s, String s1, String s2) {

    }

    @Override
    public void onException(int i) {

    }
}

```


