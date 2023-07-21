import tensorflow as tf

# Check if TensorFlow is built with GPU support
if tf.test.is_built_with_cuda():
    print("TensorFlow is built with CUDA support")

# Check if GPU is available and used by TensorFlow
if tf.test.is_gpu_available():
    print("GPU is available and used by TensorFlow")
    print("GPU devices:")
    for device in tf.config.list_physical_devices("GPU"):
        print(device)
else:
    print("No GPU available or not used by TensorFlow")
