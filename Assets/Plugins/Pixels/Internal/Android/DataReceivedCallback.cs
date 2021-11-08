using UnityEngine;

namespace Systemic.Unity.BluetoothLE.Internal.Android
{
    internal sealed class DataReceivedCallback : AndroidJavaProxy
    {
        NativeValueRequestResultHandler<byte[]> _onDataReceived;

        public DataReceivedCallback(NativeValueRequestResultHandler<byte[]> onDataReceived)
            : base("no.nordicsemi.android.ble.callback.DataReceivedCallback")
            => _onDataReceived = onDataReceived;

        void onDataReceived(AndroidJavaObject device, AndroidJavaObject data)
        {
            //Debug.Log($"{RequestOperation.SubscribeCharacteristic} ==> onDataReceived");
            using var javaArray = data.Call<AndroidJavaObject>("getValue");
            _onDataReceived?.Invoke(JavaUtils.ToDotNetArray(javaArray), RequestStatus.Success); // No notification with error on Android
        }
    }
}
