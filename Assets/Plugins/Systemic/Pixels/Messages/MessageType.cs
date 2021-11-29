﻿
namespace Systemic.Unity.Pixels.Messages
{
    /// <summary>
    /// Lists all the Pixel dice message types.
    /// The value is used for the first byte of data in a Pixel message to identify it's type.
    /// </summary>
    /// <remarks>
    /// These message identifiers have to match up with the ones on the firmware.
    /// </remarks>
    public enum MessageType : byte
    {
        None = 0,
        WhoAreYou,
        IAmADie,
        RollState,
        Telemetry,
        BulkSetup,
        BulkSetupAck,
        BulkData,
        BulkDataAck,
        TransferAnimationSet,
        TransferAnimationSetAck,
        TransferAnimationSetFinished,
        TransferSettings,
        TransferSettingsAck,
        TransferSettingsFinished,
        TransferTestAnimationSet,
        TransferTestAnimationSetAck,
        TransferTestAnimationSetFinished,
        DebugLog,
        PlayAnimation,
        PlayAnimationEvent,
        StopAnimation,
        PlaySound,
        RequestRollState,
        RequestAnimationSet,
        RequestSettings,
        RequestTelemetry,
        ProgramDefaultAnimationSet,
        ProgramDefaultAnimationSetFinished,
        Blink,
        BlinkFinished,
        RequestDefaultAnimationSetColor,
        DefaultAnimationSetColor,
        RequestBatteryLevel,
        BatteryLevel,
        RequestRssi,
        Rssi,
        Calibrate,
        CalibrateFace,
        NotifyUser,
        NotifyUserAck,
        TestHardware,
        SetStandardState,
        SetLEDAnimationState,
        SetBattleState,
        ProgramDefaultParameters,
        ProgramDefaultParametersFinished,
        SetDesignAndColor,
        SetDesignAndColorAck,
        SetCurrentBehavior,
        SetCurrentBehaviorAck,
        SetName,
        SetNameAck,

        // Testing
        TestBulkSend,
        TestBulkReceive,
        SetAllLEDsToColor,
        AttractMode,
        PrintNormals,
        PrintA2DReadings,
        LightUpFace,
        SetLEDToColor,
        DebugAnimationController,
    }
}
