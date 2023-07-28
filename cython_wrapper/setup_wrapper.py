from setuptools import setup
from Cython.Build import cythonize
import os

setup(
    name="WPK Converter",
    ext_modules=cythonize("cython_wrapper/toWPKConverterWrapper.pyx"),
    zip_safe=False,
)